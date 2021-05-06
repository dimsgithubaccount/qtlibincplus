#include "ringbuffer.h"
#include <atomic>
#include <cstring>
#include <QDebug>
#include <QMap>

#define VERIFY_RET(condition) if(!(condition)) return
#define VERIFY_RETVAL(condition, retval) if(!(condition)) return retval

#define FAST_REFERENCE
typedef QMap<char*, int> ItemReferenceMap;
static ItemReferenceMap referenceCount;

struct ItemBlock{
    char *base = nullptr;
    int _count = 0;
    int _item_size = 1;

    ItemBlock(int itemcount = 0, int itemsize = 1) { initialize(itemcount, itemsize); }

    ItemBlock(const ItemBlock &other)
    {
#ifndef FAST_REFERENCE
        initialize(other.memorySize(), other._item_size);
        std::memcpy(base, other.base, _count * _item_size);
#else
        base = other.base;
        _count = other._count;
        _item_size = other._item_size;
        int count = referenceCount.value(base);
        referenceCount.insert(base, ++count);
#endif
    }

    ItemBlock& operator= (const ItemBlock &other)
    {
#ifndef FAST_REFERENCE
        if(base)
            delete[] base;
        initialize(other.memorySize(), other._item_size);
        std::memcpy(base, other.base, _count * _item_size);
#else
        if(referenceCount.contains(base))
            base_change();
        base = other.base;
        _count = other._count;
        _item_size = other._item_size;
        int count = referenceCount.value(base);
        referenceCount.insert(base, ++count);
#endif
        return *this;
    }

    ~ItemBlock()
    {
#ifndef FAST_REFERENCE
        if(base)
            delete[] base;
#else
        VERIFY_RET(base);
        base_change();
#endif
        base = nullptr;
    }

    bool isValid() const { return base && memorySize() > 0; }
    qint32 memorySize() const { return _item_size * _count; }
private:
    inline void initialize(int count, int itemsize)
    {
        VERIFY_RET(count > 0 && itemsize > 0);
        try{
            base = new char[(_count = count) * (_item_size = itemsize)];
        }catch(std::bad_alloc &e)
        {
            qCritical() << e.what();
            _count = 0;
            base = nullptr;
        };
        if(base)
            referenceCount.insert(base, 1);
    }
#ifdef FAST_REFERENCE
    inline void base_change()
    {
        int count = referenceCount.value(base);
        if(--count > 0)
            referenceCount.insert(base, count);
        else
        {
            referenceCount.remove(base);
            delete[] base;
        }
    }
#endif
};

typedef QList<ItemBlock> ItemBlockList, RingBuffer2D;

#define DEFAULT_INCREMENT 8 * 1024 * 1024

class CRingBufferImplPri
{
    RingBuffer2D _buffer;
    std::atomic<qint64> _read_pos, _write_pos;
    qint32 _auto_increment;
    qint32 _item_size;
    std::atomic<qint64> _item_count;
    std::atomic_flag _emergency_lock = ATOMIC_FLAG_INIT;
    bool _bincrement;
    bool _binit;
    bool _work;

    CRingBufferImplPri();
    CRingBufferImplPri(const CRingBufferImplPri &other);
    CRingBufferImplPri& operator= (const CRingBufferImplPri &other);
    friend class CRingBufferImpl;
};

CRingBufferImplPri::CRingBufferImplPri()
{
    _read_pos = _write_pos = 0;
    _auto_increment = DEFAULT_INCREMENT;
    _item_size = 1;
    _item_count = 0;
    _bincrement = true;
    _binit = false;
    _work = true;
}

CRingBufferImplPri::CRingBufferImplPri(const CRingBufferImplPri &other)
{
    _buffer = other._buffer;
    qint64 tmpval;
    tmpval = other._read_pos;
    _read_pos = tmpval;
    tmpval = other._write_pos;
    _write_pos = tmpval;
    _auto_increment = other._auto_increment;
    _item_size = other._item_size;
    tmpval = other._item_count;
    _item_count = tmpval;
//    _emergency_lock;
    _bincrement = other._bincrement;
    _binit = other._binit;
    _work = other._work;
}

CRingBufferImplPri &CRingBufferImplPri::operator=(const CRingBufferImplPri &other)
{
    _buffer = other._buffer;
    qint64 tmpval;
    tmpval = other._read_pos;
    _read_pos = tmpval;
    tmpval = other._write_pos;
    _write_pos = tmpval;
    _auto_increment = other._auto_increment;
    _item_size = other._item_size;
    tmpval = other._item_count;
    _item_count = tmpval;
//    _emergency_lock;
    _bincrement = other._bincrement;
    _binit = other._binit;
    _work = other._work;
    return *this;
}

CRingBufferImpl::CRingBufferImpl(qint32 increment, qint32 itemsize)
    : _d(new CRingBufferImplPri)
{
    _d->_binit = increment > 0;
    if(_d->_binit)
        _d->_auto_increment = increment;
    _d->_item_size = itemsize;
}

CRingBufferImpl::CRingBufferImpl(const CRingBufferImpl &other)
    : _d(new CRingBufferImplPri(*other._d))
{

}

CRingBufferImpl &CRingBufferImpl::operator=(const CRingBufferImpl &other)
{
    *_d = *other._d;
    return *this;
}

CRingBufferImpl::~CRingBufferImpl()
{
    delete _d;
}

void CRingBufferImpl::setItemSize(qint32 itemsize)
{
    VERIFY_RET(!_d->_binit);
    _d->_item_size = itemsize;
    _d->_binit = true;
}

void CRingBufferImpl::create(qint32 blockcount)
{
    if(!_d->_binit)
        _d->_binit = true;
    releaseBuffer();
    while (blockcount-- > 0) {
        ItemBlock block(_d->_auto_increment, _d->_item_size);
        VERIFY_RET(block.isValid());
        _d->_buffer << block;
    }
}

qint64 CRingBufferImpl::itemCount() const
{
    return _d->_item_count;
}

qint64 CRingBufferImpl::size() const
{
    return max_pos();
}

qint32 CRingBufferImpl::blockCount() const
{
    return _d->_buffer.size();
}

qint32 CRingBufferImpl::increment() const
{
    return _d->_auto_increment;
}

bool CRingBufferImpl::isNull() const
{
    return _d->_buffer.isEmpty();
}

bool CRingBufferImpl::isEmpty() const
{
    return _d->_item_count == 0;
}

int CRingBufferImpl::read(void *varray, int readsize, int arraysize)
{
    char *array = static_cast<char*>(varray);
    if(arraysize == 0) arraysize = readsize;
    VERIFY_RETVAL(_d->_work && _d->_item_count > 0 && arraysize > 0 && readsize > 0 && array, -1);
    if(!_d->_binit)
        _d->_binit = true;
    const int popsize = (readsize < arraysize ? readsize : arraysize);
    int arrindex = 0;
    while (_d->_item_count > 0 && arrindex < popsize) {
        if(_d->_bincrement)
            while(_d->_emergency_lock.test_and_set(std::memory_order_acquire));
        int curdim = _d->_read_pos / _d->_auto_increment;
        int offset = _d->_read_pos % _d->_auto_increment;
        ItemBlock &block = _d->_buffer[curdim];
        int readable = _d->_auto_increment - offset;
        int remains = arraysize - arrindex;
        int copylen = (readable < remains ? readable : remains);
        qint64 count = _d->_item_count;
        if(copylen > count)
            copylen = count;
        std::memcpy(array + arrindex * _d->_item_size,
                    block.base + offset * _d->_item_size,
                    copylen * _d->_item_size);
        _d->_read_pos += copylen;
        _d->_item_count -= copylen;
        arrindex += copylen;
        if(_d->_read_pos >= max_pos())
            _d->_read_pos = 0;
        if(_d->_bincrement)
            _d->_emergency_lock.clear(std::memory_order_release);
    }
    return arrindex;
}

int CRingBufferImpl::write(const void *varray, int arraysize)
{
    const char *array = static_cast<const char*>(varray);
    VERIFY_RETVAL(_d->_work && array && arraysize > 0, -1);
    if(!_d->_binit)
        _d->_binit = true;
    if(_d->_buffer.isEmpty() && _d->_bincrement)
    {
        ItemBlock block(_d->_auto_increment, _d->_item_size);
        VERIFY_RETVAL(block.isValid(), -1);
        _d->_buffer << block;
    }
    int appendsize = 0;
    while(appendsize < arraysize){
        if(_d->_write_pos == max_pos())
            _d->_write_pos = 0;
        int curdim = _d->_write_pos / _d->_auto_increment;
        int offset = _d->_write_pos % _d->_auto_increment;
        ItemBlock &block = _d->_buffer[curdim];
#if ONEBYONE
        while(offset < _auto_increment && appendsize < arraysize && remains() > 0) {
            block.base[offset++] = array[appendsize++];
            ++_item_count;
            ++_write_pos.atomic_pos;
        }
#else
        int writeable = _d->_auto_increment - offset;
        int remains = arraysize - appendsize;
        int copylen = (writeable < remains ? writeable : remains);
        int r = this->remains();
        if(copylen > r)
            copylen = r;
        std::memcpy(block.base + offset * _d->_item_size,
                    array + appendsize * _d->_item_size,
                    copylen * _d->_item_size);
        _d->_item_count += copylen;
        _d->_write_pos += copylen;
        offset += copylen;
        appendsize += copylen;
#endif
        remains = arraysize - appendsize;
        if(remains > 0 && 0 == this->remains())
        {
            VERIFY_RETVAL(_d->_bincrement, appendsize);
            int dimcount = remains / _d->_auto_increment + (remains % _d->_auto_increment ? 1 : 0);
            while(_d->_emergency_lock.test_and_set(std::memory_order_acquire));
            for(int i = 0; i < dimcount; ++i) {
                ItemBlock block(_d->_auto_increment, _d->_item_size);
                if(!block.isValid())
                {
                    _d->_emergency_lock.clear(std::memory_order_release);
                    return appendsize;
                }
                _d->_buffer.insert(curdim + 1, block);
            }
            if(_d->_write_pos <= _d->_read_pos)
            {
                int rddim = _d->_read_pos / _d->_auto_increment;
                if(curdim == rddim)
                {
                    int rdoffset = _d->_read_pos % _d->_auto_increment;
                    ItemBlock &rawblock = _d->_buffer[curdim];
                    ItemBlock &newblock = _d->_buffer[curdim + dimcount];
                    std::memcpy(newblock.base + rdoffset * _d->_item_size,
                                rawblock.base + rdoffset * _d->_item_size,
                                (_d->_auto_increment - rdoffset) * _d->_item_size);
                }
                _d->_read_pos += dimcount * _d->_auto_increment;
            }
            _d->_emergency_lock.clear(std::memory_order_release);
        }
    }
    return appendsize;
}

void CRingBufferImpl::releaseBuffer()
{
    while(_d->_emergency_lock.test_and_set(std::memory_order_acquire));
    _d->_item_count = 0;
    _d->_read_pos = 0;
    _d->_write_pos = 0;
    RingBuffer2D empty;
    empty.swap(_d->_buffer);
    _d->_emergency_lock.clear(std::memory_order_release);
//#pragma omp parallel for
//    for(int i = 0; i < empty.size(); ++i)
//    {
//        if(nullptr != empty[i].base)
//            delete[] empty[i].base;
//    }
    qDebug() << empty.size();
}

void CRingBufferImpl::forbid()
{
    VERIFY_RET(_d->_work);
    while(_d->_emergency_lock.test_and_set(std::memory_order_acquire));
    _d->_work = false;
}

void CRingBufferImpl::recover()
{
    VERIFY_RET(!_d->_work);
    _d->_emergency_lock.clear(std::memory_order_release);
    _d->_work = true;
}

qint64 CRingBufferImpl::max_pos() const
{
    return _d->_buffer.size() * qint64(_d->_auto_increment);
}

qint64 CRingBufferImpl::remains() const
{
    return max_pos() - _d->_item_count;
}
