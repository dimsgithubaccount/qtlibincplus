#ifndef CRINGBUFFER_H
#define CRINGBUFFER_H

#include <QObject>

#include <QtCore/qglobal.h>

#ifndef LIBTEST
#if defined(LIBSHARED_RINGBUFFER_EXPORT)
#  define LIBSHARED_RINGBUFFER_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHARED_RINGBUFFER_EXPORT Q_DECL_IMPORT
#endif
#else
#  define LIBSHARED_RINGBUFFER_EXPORT
#endif

class CRingBufferImplPri;

class LIBSHARED_RINGBUFFER_EXPORT CRingBufferImpl
{
    CRingBufferImplPri *const _d;
public:
    explicit CRingBufferImpl(qint32 increment = 0, qint32 itemsize = 1);

    CRingBufferImpl(const CRingBufferImpl &other);

    CRingBufferImpl& operator= (const CRingBufferImpl &other);

    ~CRingBufferImpl();

    ///
    /// \brief setItemSize: Initialize the size of each same item, once only!
    /// \param itemsize
    ///
    void setItemSize(qint32 itemsize);

    ///
    /// \brief create: Create a new ring-buffer contains blockcount pieces of memories,
    /// it will destroy the existing buffer before
    /// \param blockcount
    ///
    void create(qint32 blockcount);

    ///
    /// \brief itemCount: Returns current number of items in ring-buffer
    /// \return
    ///
    qint64 itemCount() const;

    ///
    /// \brief size: Returns the max item count
    /// \return
    ///
    qint64 size() const;

    ///
    /// \brief blockCount: Returns the pieces of memory block in ring-buffer,
    /// this value is fixed if ring-buffer doesn't support auto-increment
    /// \return
    ///
    qint32 blockCount() const;

    ///
    /// \brief increment: Returns the auto-increment of ring-buffer,
    /// this value is fixed after initialized
    /// \return
    ///
    qint32 increment() const;

    ///
    /// \brief isNull: Returns true if the ring-buffer has no piece of memory block, otherwise returns false
    /// \return
    ///
    bool isNull() const;

    ///
    /// \brief isEmpty: Returns true if the ring-buffer has no item, otherwise returns false
    /// \return
    ///
    bool isEmpty() const;

    ///
    /// \brief clear: Clear items from the ring-buffer
    ///
    void clear();

    ///
    /// \brief releaseBuffer: Clear and destroy the ring-buffer
    ///
    void releaseBuffer();

    ///
    /// \brief forbid: Disable ring-buffer functionality
    ///
    void forbid();

    ///
    /// \brief recover: Restore the function of the ring-buffer
    ///
    void recover();
protected:
    int read(void *array, int readsize, int arraysize = 0);

    int write(const void *array, int arraysize);
private:
    inline qint64 max_pos() const;

    inline qint64 remains() const;
};

template<class T>
class LIBSHARED_RINGBUFFER_EXPORT CRingBuffer : public QObject, public CRingBufferImpl
{
public:
    explicit CRingBuffer(QObject *parent = nullptr);

    explicit CRingBuffer(qint32 increment, qint32 blockcount, QObject *parent = nullptr);

    CRingBuffer(const CRingBuffer &other);

    CRingBuffer& operator= (const CRingBuffer &other);

    ~CRingBuffer();

    ///
    /// \brief read: Read items from ring-buffer into array
    /// \param array
    /// \param readsize
    /// \param arraysize
    /// \return
    ///
    int read(T* array, int readsize, int arraysize = 0);

    ///
    /// \brief write: Write items from array into ring-buffer
    /// \param array
    /// \param arraysize
    /// \return
    ///
    int write(const T *array, int arraysize);
};

template<class T>
CRingBuffer<T>::CRingBuffer(QObject *parent)
    : QObject(parent),
      CRingBufferImpl(0, sizeof (T))
{

}

template<class T>
CRingBuffer<T>::CRingBuffer(qint32 increment, qint32 blockcount, QObject *parent)
    : QObject(parent),
      CRingBufferImpl(increment, sizeof (T))
{
    create(blockcount);
}

template<class T>
CRingBuffer<T>::CRingBuffer(const CRingBuffer &other)
    : QObject(other.parent()),
      CRingBufferImpl(other)
{

}

template<class T>
CRingBuffer<T> &CRingBuffer<T>::operator=(const CRingBuffer &other)
{
    CRingBufferImpl::operator=(other);
    setParent(other.parent());
}

template<class T>
CRingBuffer<T>::~CRingBuffer()
{

}

template<class T>
int CRingBuffer<T>::read(T *array, int readsize, int arraysize)
{
    void *varrary = static_cast<void*>(array);
    return CRingBufferImpl::read(varrary, readsize * sizeof (T), arraysize * sizeof (T));
}

template<class T>
int CRingBuffer<T>::write(const T *array, int arraysize)
{
    const void *varray = static_cast<const void*>(array);
    return CRingBufferImpl::write(varray, arraysize * sizeof (T));
}

typedef CRingBuffer<char>   CharRingBuffer;
typedef CRingBuffer<qint16> Int16RingBuffer;
typedef CRingBuffer<int>    IntRingBuffer;
typedef CRingBuffer<qint64> Int64RingBuffer;
typedef CRingBuffer<float>  FloatRingBuffer;
typedef CRingBuffer<double> DoubleRingBuffer;
    /// 注意非基础类型以及非由基础类型组成的不带继承关系的结构体和类，只要它们是可复制构造的，
    /// 原则上均可以作为缓冲区元素，然而，这没有意义。





































//template <class T>
//class LIBSHARED_RINGBUFFER_EXPORT CRingBufferGood : public QObject
//{
//#define VERIFY_RET(condition) if(!(condition)) return
//#define VERIFY_RETVAL(condition, retval) if(!(condition)) return retval
//#define DEFAULT_INCREMENT 8 * 1024 * 1024
//protected:
//    struct ItemCoordinate{
//        std::atomic<qint64> atomic_pos;

//        ItemCoordinate(qint64 coordinate = 0) { atomic_pos = coordinate; }
//        ItemCoordinate(const ItemCoordinate &other) { atomic_pos = other.atomic_pos; }

//        void clear() { atomic_pos = 0; }
//    };

//    struct ItemBlock{
//        T *base = nullptr;
//        int _size = 0;

//        ItemBlock(int size = 0) { initialize(size); }

//        ItemBlock& initialize(int size = 0)
//        {
//            VERIFY_RETVAL(size > 0, *this);
//            try{
//                base = new T[_size = size];
//            }catch(std::bad_alloc &e)
//            {
//                QMessageLogger(__FILE__, __LINE__, __FUNCTION__).fatal(e.what());
//                _size = 0;
//            };
//            return *this;
//        }

//        bool isValid() const { return base && _size > 0; }
//        qint32 size() const { return sizeof (T) * _size; }
//    };

//    typedef QList<ItemBlock> ItemBlockList, RingBuffer2D;

//    RingBuffer2D _buffer;
//    ItemCoordinate _read_pos;
//    ItemCoordinate _write_pos;
//    qint32 _auto_increment;
//    std::atomic<qint64> _item_count;
//    std::atomic_flag _emergency_lock = ATOMIC_FLAG_INIT;
//    bool _bincrement = true;
//    bool _work;
//public:
//    explicit CRingBufferGood(QObject *parent = nullptr);

//    explicit CRingBufferGood(int blocksize, int blockcount, QObject *parent = nullptr);

//    CRingBufferGood(const CRingBufferGood &other);

//    ~CRingBufferGood();

//    qint64 itemCount() const { return _item_count; }

//    qint64 size() const { return max_pos(); }

//    qint32 blocksize() const { return _buffer.size(); }

//    qint32 inrement() const { return _bincrement ? _auto_increment : 0; }

//    bool isNull() const { return _buffer.isEmpty(); }

//    bool isEmpty() const { return _item_count == 0; }

//    void setAutoIncrementOpt(bool bAutoIncrement);

//    void releaseBuffer();

//    int read(T* array, int readsize, int arraysize = 0);    ///线程#1读和

//    int write(const T *array, int arraysize);               ///         线程#2写是允许双线程操作的，当读或写含有多个操作线程时，这些线程的读或写操作需要加互斥条件

//    void forbid();  ///this function forbid read-write operation until called recover()

//    void recover();
//protected:
//    inline qint64 max_pos() const { return (_buffer.size() * _auto_increment); }

//    inline qint64 remains() const { return max_pos() - _item_count; }
//};

//typedef CRingBuffer/*Good*/<short>   CharRingBuffer;
//typedef CRingBufferGood<qint16> Int16RingBuffer;
//typedef CRingBufferGood<int>    IntRingBuffer;
//typedef CRingBufferGood<qint64> Int64RingBuffer;
//typedef CRingBufferGood<float>  FloatRingBuffer;
//typedef CRingBufferGood<double> DoubleRingBuffer;
//    /// 注意非基础类型以及非由基础类型组成的不带继承关系的结构体和类，只要它们是可复制构造的，
//    /// 原则上均可以作为缓冲区元素，然而，这没有意义。




//template<class T>
//CRingBufferGood<T>::CRingBufferGood(QObject *parent) : QObject(parent)
//{
//    _item_count = 0;
//    _auto_increment = DEFAULT_INCREMENT;
//    _work = true;
//}

//template<class T>
//CRingBufferGood<T>::CRingBufferGood(int blocksize, int blockcount, QObject *parent) : QObject(parent)
//{
//    if(blocksize < 2)
//    {
//        blocksize = DEFAULT_INCREMENT;
//        QMessageLogger(__FILE__, __LINE__, __FUNCTION__).warning("the ringbuffer is too small! reset increment as default size.");
//    }
//    _item_count = 0;
//    _auto_increment = blocksize;
//    while (blockcount-- > 0) {
//        ItemBlock block(blocksize);
//        VERIFY_RET(block.isValid());
//        _buffer << block;
//    }
//    _work = true;
//}

//template<class T>
//CRingBufferGood<T>::CRingBufferGood(const CRingBufferGood &other) : QObject(other.parent())
//{
//    releaseBuffer();
//    for(int i = 0; i < other._buffer.size(); ++i)
//    {
//        const ItemBlock &v = other._buffer[i];
//        ItemBlock block(v._size);
//        VERIFY_RET(block.isValid());
//        for(int j = 0; j < v._size; ++j)
//        {
//            block.base[j] = v.base[j];
//        }
//        _buffer << block;
//    }
//    qint64 tmpval = other._read_pos.atomic_pos;
//    _read_pos.atomic_pos = tmpval;
//    tmpval = other._write_pos.atomic_pos;
//    _write_pos.atomic_pos = tmpval;
//    _auto_increment = other._auto_increment;
//    tmpval = other._item_count;
//    _item_count = tmpval;
//    _work = true;
//}

//template<class T>
//CRingBufferGood<T>::~CRingBufferGood()
//{
////    forbid();
////    recover();
//    releaseBuffer();
//}

////template<class T>
////void CRingBufferGood<T>::setAutoIncrementOpt(bool bAutoIncrement)
////{
//////    AutoLocker lock(_emergency_lock);
//////    _bincrement = bAutoIncrement;
////}

//#include <QDebug>

//template<class T>
//void CRingBufferGood<T>::releaseBuffer()
//{
//    while(_emergency_lock.test_and_set(std::memory_order_acquire));
//    _item_count = 0;
//    _read_pos.clear();
//    _write_pos.clear();
//    RingBuffer2D empty;
//    empty.swap(_buffer);
//    _emergency_lock.clear(std::memory_order_release);
//    for(int i = 0; i < empty.size(); ++i)
//    {
//        if(nullptr != empty[i].base)
//            delete[] empty[i].base;
//    }
//    qDebug() << empty.size();
//}

//#define ONEBYONE 0

//template<class T>
//int CRingBufferGood<T>::read(T* array, int readsize, int arraysize)
//{
//    if(arraysize == 0) arraysize =  readsize;
//    VERIFY_RETVAL(_work && _item_count > 0 && arraysize > 0 && readsize > 0 && array, -1);
//    const int popsize = (readsize < arraysize ? readsize : arraysize);
//    int arrindex = 0;
//    while(_item_count > 0 && arrindex < popsize)
//    {
//        if(_bincrement)
//            while(_emergency_lock.test_and_set(std::memory_order_acquire));
//        int tmpdim = _read_pos.atomic_pos / _auto_increment;
//        int offset = _read_pos.atomic_pos % _auto_increment;
//        ItemBlock &block = _buffer[tmpdim];
//#if ONEBYONE
//        while(offset < _auto_increment && arrindex < popsize && _item_count > 0)
//        {
//            array[arrindex++] = block.base[offset];
//            ++offset;
//            ++_read_pos.atomic_pos;
//            --_item_count;
//        }
//#else
//        int readable = _auto_increment - offset;
//        int remains = arraysize - arrindex;
//        int copylen = (readable < remains ? readable : remains);
//        qint64 count = _item_count;
//        if(copylen > count)
//            copylen = count;
//        std::memcpy(array + arrindex * sizeof (T),
//                    block.base + offset * sizeof (T),
//                    copylen * sizeof (T));
//        _read_pos.atomic_pos += copylen;
//        _item_count -= copylen;
//        arrindex += copylen;
//#endif
//        if(_read_pos.atomic_pos >= max_pos())
//            _read_pos.atomic_pos = 0;
//        if(_bincrement)
//            _emergency_lock.clear(std::memory_order_release);
//    }
//    return arrindex;
//}

//template<class T>
//int CRingBufferGood<T>::write(const T *array, int arraysize)
//{
//    VERIFY_RETVAL(_work && array && arraysize > 0, -1);
//    if(_buffer.isEmpty())
//    {
//        ItemBlock block(_auto_increment);
//        VERIFY_RETVAL(block.isValid(), -1);
//        _buffer << block;
//    }
//    int appendsize = 0;
//    while(appendsize < arraysize){
//        if(_write_pos.atomic_pos == max_pos())
//            _write_pos.atomic_pos = 0;
//        int curdim = _write_pos.atomic_pos / _auto_increment;
//        int offset = _write_pos.atomic_pos % _auto_increment;
//        ItemBlock &block = _buffer[curdim];
//#if ONEBYONE
//        while(offset < _auto_increment && appendsize < arraysize && remains() > 0) {
//            block.base[offset++] = array[appendsize++];
//            ++_item_count;
//            ++_write_pos.atomic_pos;
//        }
//#else
//        int writeable = _auto_increment - offset;
//        int remainlen = arraysize - appendsize;
//        int copylen = (writeable < remainlen ? writeable : remainlen);
//        int r = remains();
//        if(copylen > r)
//            copylen = r;
//        std::memcpy(block.base + offset * sizeof (T),
//                    array + appendsize * sizeof (T),
//                    copylen * sizeof (T));
//        _item_count += copylen;
//        _write_pos.atomic_pos += copylen;
//        offset += copylen;
//        appendsize += copylen;
//#endif
//        int minsize = arraysize - appendsize;
//        if(minsize > 0 && 0 == remains())
//        {
//            VERIFY_RETVAL(_bincrement, appendsize);
//            int dimcount = minsize / _auto_increment + (minsize % _auto_increment ? 1 : 0);
//            while(_emergency_lock.test_and_set(std::memory_order_acquire));
//            bool bWmR = _write_pos.atomic_pos > _read_pos.atomic_pos;
//            for(int i = 0; i < dimcount; ++i) {
//                ItemBlock block(_auto_increment);
//                if(!block.isValid())
//                {
//                    _emergency_lock.clear(std::memory_order_release);
//                    return appendsize;
//                }
//                _buffer.insert(curdim + 1, block);
//            }
//            if(!bWmR)
//            {
//                int rddim = _read_pos.atomic_pos / _auto_increment;
//                if(curdim == rddim)
//                {
//                    int rdoffset = _read_pos.atomic_pos % _auto_increment;
//                    ItemBlock &rawblock = _buffer[curdim];
//                    ItemBlock &newblock = _buffer[curdim + dimcount];
//                    std::memcpy(newblock.base + rdoffset * sizeof (T),
//                                rawblock.base + rdoffset * sizeof (T),
//                                (_auto_increment - rdoffset) * sizeof (T));
//                }
//                _read_pos.atomic_pos += dimcount *_auto_increment;
//            }
//            _emergency_lock.clear(std::memory_order_release);
//        }
//    }
//    return appendsize;
//}

//template<class T>
//void CRingBufferGood<T>::forbid()
//{
//    VERIFY_RET(_work);
//    while(_emergency_lock.test_and_set(std::memory_order_acquire));
//    _work = false;
//}

//template<class T>
//void CRingBufferGood<T>::recover()
//{
//    VERIFY_RET(!_work);
//    _emergency_lock.clear(std::memory_order_release);
//    _work = true;
//}

#endif // CRINGBUFFER_H
