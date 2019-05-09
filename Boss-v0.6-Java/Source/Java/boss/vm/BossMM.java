package boss.vm;

import java.util.ArrayList;

public class BossMM
{
  final static public int
    DATA_TYPE_OBJECT = 0,
    DATA_TYPE_REAL64 = 1,
    DATA_TYPE_REAL32 = 2,
    DATA_TYPE_INT32  = 3,
    DATA_TYPE_BYTE   = 4,

    DATA_TYPE_COUNT  = 5;

  final static public int
    DATA          = 0,
    DATA_COUNT    = 1,
    DATA_TYPE     = 2,
    STATUS        = 3,  // 0=freed allocation, >0 is used, untraced; <0 is used, traced
    PREV_ALLOC    = 4,
    NEXT_ALLOC    = 5,
    PREV_OBJECT   = 6,
    NEXT_OBJECT   = 7,

    HEADER_SIZE   = 8;

  public int[] lastAlloc        = new int[ DATA_TYPE_COUNT ];
  public int[] usedObjectsBegin = new int[ DATA_TYPE_COUNT ];
  public int[] usedObjectsEnd   = new int[ DATA_TYPE_COUNT ];
  public int[] freeObjectsBegin = new int[ DATA_TYPE_COUNT ];
  public int[] freeObjectsEnd   = new int[ DATA_TYPE_COUNT ];

  public BossInt32List     headerData;
  public ArrayList<Object> objectData;
  public BossReal64List    real64Data;
  public BossReal32List    real32Data;
  public BossInt32List     int32Data;
  public BossByteList      byteData;
  public int               firstFreeObjectHeader;  // unspecialized - not attached int objects etc

  public BossMM()
  {
    headerData = new BossInt32List( 1 );
    objectData = new ArrayList<Object>( 1 );
    real64Data = new BossReal64List( 1 );
    real32Data = new BossReal32List( 1 );
    int32Data  = new BossInt32List( 1 );
    byteData   = new BossByteList( 1 );

    headerData.add( 0 );

    for (int i=0; i<DATA_TYPE_COUNT; ++i)
    {
      _configureList( usedObjectsBegin, usedObjectsEnd, i );
      _configureList( freeObjectsBegin, freeObjectsEnd, i );
    }
  }

  public int allocateByteDataObject( int count )
  {
    int obj = _allocateByRecyclingExistingObject( count, DATA_TYPE_BYTE );

    if (obj == 0)
    {
      obj = _allocateNewObject( byteData.count, DATA_TYPE_BYTE );
      byteData.expand( count );
    }

    _finishObjectAllocation( obj, count, DATA_TYPE_BYTE );
    return obj;
  }

  public int allocateInt32DataObject( int count )
  {
    int obj = _allocateByRecyclingExistingObject( count, DATA_TYPE_INT32 );

    if (obj == 0)
    {
      obj = _allocateNewObject( int32Data.count, DATA_TYPE_INT32 );
      int32Data.expand( count );
    }

    _finishObjectAllocation( obj, count, DATA_TYPE_INT32 );
    return obj;
  }

  public int allocateObjectDataObject( int count )
  {
    int obj = _allocateByRecyclingExistingObject( count, DATA_TYPE_OBJECT );

    if (obj == 0)
    {
      obj = _allocateNewObject( objectData.size(), DATA_TYPE_OBJECT );
      objectData.ensureCapacity( objectData.size() + count );
      for (int i=count; --i>=0;)
      {
        objectData.add( null );
      }
    }

    _finishObjectAllocation( obj, count, DATA_TYPE_OBJECT );
    return obj;
  }

  public int allocateReal64DataObject( int count )
  {
    int obj = _allocateByRecyclingExistingObject( count, DATA_TYPE_REAL64 );

    if (obj == 0)
    {
      obj = _allocateNewObject( real64Data.count, DATA_TYPE_REAL64 );
      real64Data.expand( count );
    }

    _finishObjectAllocation( obj, count, DATA_TYPE_REAL64 );
    return obj;
  }

  public int allocateReal32DataObject( int count )
  {
    int obj = _allocateByRecyclingExistingObject( count, DATA_TYPE_REAL32 );

    if (obj == 0)
    {
      obj = _allocateNewObject( real32Data.count, DATA_TYPE_REAL32 );
      real32Data.expand( count );
    }

    _finishObjectAllocation( obj, count, DATA_TYPE_REAL32 );
    return obj;
  }

  public int count( int obj )
  {
    return headerData.get( obj+DATA_COUNT );
  }

  public int data( int obj )
  {
    return headerData.get( obj+DATA );
  }

  public void freeObject( int obj )
  {
//System.out.println( "Freeing object " + obj + "(" + count(obj) + ")" );
    headerData.set( obj+STATUS, 0 );

    // clear object data
    int data = headerData.get( obj+DATA );
    int count = headerData.get( obj+DATA_COUNT );
    switch (headerData.get(obj+DATA_TYPE))
    {
      case DATA_TYPE_OBJECT:
        for (int i=data+count; --i>=data;) objectData.set( i, null );
        break;
      case DATA_TYPE_REAL64:
        for (int i=data+count; --i>=data;) real64Data.set( i, 0 );
        break;
      case DATA_TYPE_REAL32:
        for (int i=data+count; --i>=data;) real32Data.set( i, 0 );
        break;
      case DATA_TYPE_INT32:
        for (int i=data+count; --i>=data;) int32Data.set( i, 0 );
        break;
      case DATA_TYPE_BYTE:
        for (int i=data+count; --i>=data;) byteData.set( i, (byte)0 );
        break;
      default:
        System.out.println( "*** Warning: no data clear defined for current data type in BossMM. ***" );
    }

    // remove from object list
    _unlink( obj, PREV_OBJECT, NEXT_OBJECT );

    // merge with adjacent allocations
    int dataType = headerData.get( obj+DATA_TYPE );
    int prev = headerData.get( obj+PREV_ALLOC );
    int next = headerData.get( obj+NEXT_ALLOC );

    if (next > 0 && headerData.get(next+STATUS) == 0)
    {
      // Merge this free alloc with next
      headerData.set( obj+DATA_COUNT, headerData.get(obj+DATA_COUNT) + headerData.get(next+DATA_COUNT) );
      headerData.set( obj+NEXT_ALLOC, next );
      _unlink( next, PREV_OBJECT, NEXT_OBJECT );
      _unlink( next, PREV_ALLOC, NEXT_ALLOC );
      _freeObjectHeader( next );
      if (next == lastAlloc[dataType]) lastAlloc[dataType] = obj;
      next = headerData.get( obj+NEXT_ALLOC );
    }

    if (prev > 0 && headerData.get(prev+STATUS) == 0)
    {
      // Merge this free alloc with previous
      headerData.set( prev+DATA_COUNT, headerData.get(prev+DATA_COUNT) + headerData.get(obj+DATA_COUNT) );
      _unlink( obj, PREV_OBJECT, NEXT_OBJECT );
      _unlink( obj, PREV_ALLOC, NEXT_ALLOC );
      _freeObjectHeader( obj );
      if (obj == lastAlloc[dataType]) lastAlloc[dataType] = prev;
    }
    else
    {
      _insert( obj, freeObjectsEnd[dataType], PREV_OBJECT, NEXT_OBJECT );
    }
  }

  public int _allocateNewObject( int data, int dataType )
  {
    int obj = _allocateObjectHeader( dataType );
    headerData.set( obj+PREV_ALLOC, lastAlloc[dataType] );
    headerData.set( obj+NEXT_ALLOC, 0 );
    if (lastAlloc[dataType] > 0) headerData.set( lastAlloc[dataType]+NEXT_ALLOC, obj );
    lastAlloc[dataType] = obj;
    return obj;
  }

  public int _allocateByRecyclingExistingObject( int count, int dataType )
  {
    int obj = headerData.get( freeObjectsBegin[dataType]+NEXT_OBJECT );
    int end = freeObjectsEnd[ dataType ];
    while (obj != end)
    {
      int curCount = headerData.get( obj + DATA_COUNT );
      if (curCount >= count)
      {
        if (curCount > count)
        {
          // Split allocation into two objects
          int next = _allocateObjectHeader( dataType );
          headerData.set( obj+DATA_COUNT, count );
          headerData.set( next+DATA_COUNT, curCount - count );
          headerData.set( next+DATA, headerData.get(obj+DATA) + count );
          _append( next, obj, PREV_ALLOC, NEXT_ALLOC );
          _append( next, obj, PREV_OBJECT, NEXT_OBJECT );
          if (obj == lastAlloc[dataType]) lastAlloc[dataType] = next;
        }
        _unlink( obj, PREV_OBJECT, NEXT_OBJECT );
        return obj;
      }
      obj = headerData.get( obj+NEXT_OBJECT );
    }
    return 0;
  }

  public int _allocateObjectHeader( int dataType )
  {
    int meta = firstFreeObjectHeader;
    if (meta > 0)
    {
      firstFreeObjectHeader = headerData.get( meta+NEXT_OBJECT );
      headerData.set( meta+NEXT_OBJECT, 0 );
    }
    else
    {
      meta = headerData.count;
      headerData.expand( HEADER_SIZE );
    }
    headerData.set( meta+DATA_TYPE, dataType );
    headerData.set( meta+NEXT_OBJECT, 0 );
    headerData.set( meta+PREV_OBJECT, 0 );
    return meta;
  }

  public void _append( int obj, int prev, int prevLink, int nextLink )
  {
    // inserts obj before end-of-list marker
    int next = headerData.get( prev+nextLink );
    headerData.set( prev+nextLink, obj );
    headerData.set( obj+nextLink, next );
    headerData.set( next+prevLink, obj );
    headerData.set( obj+prevLink, prev );
  }

  public void _configureList( int[] begin, int[] end, int dataType )
  {
    begin[dataType] = _allocateObjectHeader( dataType );
    end[dataType] = _allocateObjectHeader( dataType );
    headerData.set( begin[dataType]+NEXT_OBJECT, end[dataType] );
    headerData.set( end[dataType]+PREV_OBJECT, begin[dataType] );
    headerData.set( begin[dataType]+STATUS, -1 );  // prevent alloc merging
    headerData.set( end[dataType]+STATUS, -1 );
  }

  public void _finishObjectAllocation( int obj, int count, int dataType )
  {
    headerData.set( obj+DATA_COUNT, count );
    headerData.set( obj+STATUS, 1 );

    _insert( obj, usedObjectsEnd[dataType], PREV_OBJECT, NEXT_OBJECT );
  }

  public void _freeObjectHeader( int meta )
  {
    headerData.set( meta+NEXT_OBJECT, firstFreeObjectHeader );
    firstFreeObjectHeader = meta;
  }

  public void _insert( int obj, int next, int prevLink, int nextLink )
  {
    // inserts obj before next
    _append( obj, headerData.get(next+prevLink), prevLink, nextLink );
  }

  public void _unlink( int obj, int prevLink, int nextLink )
  {
    int prev = headerData.get( obj+prevLink );
    int next = headerData.get( obj+nextLink );
    if (prev > 0) headerData.set( prev+nextLink, next );
    if (next > 0) headerData.set( next+prevLink, prev );
  }
}
