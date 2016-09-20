#pragma once
#include "HookFunctions.h"


void* AllocMem(size_t sz )
{
	if( pOrgHeapAlloc)
		return pOrgHeapAlloc(GetProcessHeap(),0,sz);
	else
		return HeapAlloc( GetProcessHeap(),0,sz);
}

void DeleteMem( void* pMem )
{
	if( pOrgHeapFree)
		pOrgHeapFree(GetProcessHeap(),0,pMem);
	else
		HeapFree( GetProcessHeap(),0,pMem );
}

////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif


struct CPlexEx     // warning variable length structure
{
	CPlexEx* pNext;
#ifndef _WIN64
#if (_AFX_PACKING >= 8)
	DWORD dwReserved[1];    // align on 8 byte boundary
#endif
#endif
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlexEx* PASCAL Create(CPlexEx*& head, UINT_PTR nMax, UINT_PTR cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif


/////////////////////////////////////////////////////////////////////////////
// CPlexEx

CPlexEx* PASCAL CPlexEx::Create(CPlexEx*& pHead, UINT_PTR nMax, UINT_PTR cbElement)
{
	ASSERT(nMax > 0 && cbElement > 0);
	if (nMax == 0 || cbElement == 0)
	{
		AfxThrowInvalidArgException();
	}

	//CPlexEx* p = (CPlexEx*) new BYTE[sizeof(CPlexEx) + nMax * cbElement];
	CPlexEx* p = (CPlexEx*) AllocMem( sizeof(CPlexEx) + nMax * cbElement );
			// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void CPlexEx::FreeDataChain()     // free this one and links
{
	CPlexEx* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		CPlexEx* pNext = p->pNext;
		//delete[] bytes;
		DeleteMem( bytes);
		p = pNext;
	}
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE>

template<class TYPE, class ARG_TYPE = const TYPE&>
class CArrayEx : public CObject
{
public:
// Construction
	CArrayEx();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	const TYPE& GetAt(INT_PTR nIndex) const;
	TYPE& GetAt(INT_PTR nIndex);
	void SetAt(INT_PTR nIndex, ARG_TYPE newElement);
	const TYPE& ElementAt(INT_PTR nIndex) const;
	TYPE& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, ARG_TYPE newElement);
	INT_PTR Add(ARG_TYPE newElement);
	INT_PTR Append(const CArrayEx& src);
	void Copy(const CArrayEx& src);

	// overloaded operator helpers
	const TYPE& operator[](INT_PTR nIndex) const;
	TYPE& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount = 1);
	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void InsertAt(INT_PTR nStartIndex, CArrayEx* pNewArray);

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount

public:
	~CArrayEx();
	void Serialize(CArchive&);

};


/////////////////////////////////////////////////////////////////////////////
// CArrayEx<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
AFX_INLINE INT_PTR CArrayEx<TYPE, ARG_TYPE>::GetSize() const
	{ return m_nSize; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE INT_PTR CArrayEx<TYPE, ARG_TYPE>::GetCount() const
	{ return m_nSize; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE BOOL CArrayEx<TYPE, ARG_TYPE>::IsEmpty() const
	{ return m_nSize == 0; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE INT_PTR CArrayEx<TYPE, ARG_TYPE>::GetUpperBound() const
	{ return m_nSize-1; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArrayEx<TYPE, ARG_TYPE>::RemoveAll()
	{ SetSize(0, -1); }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::GetAt(INT_PTR nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
template<class TYPE, class ARG_TYPE>
AFX_INLINE const TYPE& CArrayEx<TYPE, ARG_TYPE>::GetAt(INT_PTR nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArrayEx<TYPE, ARG_TYPE>::SetAt(INT_PTR nIndex, ARG_TYPE newElement)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		m_pData[nIndex] = newElement; 
	else
		AfxThrowInvalidArgException();		
}
template<class TYPE, class ARG_TYPE>
AFX_INLINE const TYPE& CArrayEx<TYPE, ARG_TYPE>::ElementAt(INT_PTR nIndex) const
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::ElementAt(INT_PTR nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
template<class TYPE, class ARG_TYPE>
AFX_INLINE const TYPE* CArrayEx<TYPE, ARG_TYPE>::GetData() const
	{ return (const TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE* CArrayEx<TYPE, ARG_TYPE>::GetData()
	{ return (TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE INT_PTR CArrayEx<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE const TYPE& CArrayEx<TYPE, ARG_TYPE>::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// CArrayEx<TYPE, ARG_TYPE> out-of-line functions

template<class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::CArrayEx()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::~CArrayEx()
{
	ASSERT_VALID(this);

	if (m_pData != NULL)
	{
		for( int i = 0; i < m_nSize; i++ )
			(m_pData + i)->~TYPE();
		//delete[] (BYTE*)m_pData;
		DeleteMem( m_pData );
	}
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	if(nNewSize < 0 )
		AfxThrowInvalidArgException();

	if (nGrowBy >= 0)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			for( int i = 0; i < m_nSize; i++ )
				(m_pData + i)->~TYPE();
			//delete[] (BYTE*)m_pData;
			DeleteMem( m_pData );
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create buffer big enough to hold number of requested elements or
		// m_nGrowBy elements, whichever is larger.
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(TYPE));    // no overflow
#endif
		size_t nAllocSize = __max(nNewSize, m_nGrowBy);
		//m_pData = (TYPE*) new BYTE[(size_t)nAllocSize * sizeof(TYPE)];
		m_pData = (TYPE*) AllocMem( (size_t)nAllocSize * sizeof(TYPE));
		memset((void*)m_pData, 0, (size_t)nAllocSize * sizeof(TYPE));
		for( int i = 0; i < nNewSize; i++ )
#pragma push_macro("new")
#undef new
			::new( (void*)( m_pData + i ) ) TYPE;
#pragma pop_macro("new")
		m_nSize = nNewSize;
		m_nMaxSize = nAllocSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			memset((void*)(m_pData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
for( int i = 0; i < nNewSize-m_nSize; i++ )
#pragma push_macro("new")
#undef new
				::new( (void*)( m_pData + m_nSize + i ) ) TYPE;
#pragma pop_macro("new")
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			for( int i = 0; i < m_nSize-nNewSize; i++ )
				(m_pData + nNewSize + i)->~TYPE();
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		INT_PTR nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
		
		if(nNewMax  < m_nMaxSize)
			AfxThrowInvalidArgException();

#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		//TYPE* pNewData = (TYPE*) new BYTE[(size_t)nNewMax * sizeof(TYPE)];
		TYPE* pNewData = (TYPE*) AllocMem((size_t)nNewMax * sizeof(TYPE));

		// copy new data from old
		::ATL::Checked::memcpy_s(pNewData, (size_t)nNewMax * sizeof(TYPE),
			m_pData, (size_t)m_nSize * sizeof(TYPE));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);
		memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
		for( int i = 0; i < nNewSize-m_nSize; i++ )
#pragma push_macro("new")
#undef new
			::new( (void*)( pNewData + m_nSize + i ) ) TYPE;
#pragma pop_macro("new")

		// get rid of old stuff (note: no destructors called)
		//delete[] (BYTE*)m_pData;
		DeleteMem( m_pData );
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE, class ARG_TYPE>
INT_PTR CArrayEx<TYPE, ARG_TYPE>::Append(const CArrayEx& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself
	
	if(this == &src)
		AfxThrowInvalidArgException();

	INT_PTR nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	CopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::Copy(const CArrayEx& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	if(this != &src)
	{
		SetSize(src.m_nSize);
		CopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
	}
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::FreeExtra()
{
	ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			//pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
			pNewData = (TYPE*) AllocMem(m_nSize * sizeof(TYPE));
			// copy new data from old
			::ATL::Checked::memcpy_s(pNewData, m_nSize * sizeof(TYPE),
				m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
		//delete[] (BYTE*)m_pData;
		DeleteMem(m_pDate);
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::SetAtGrow(INT_PTR nIndex, ARG_TYPE newElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	
	if(nIndex < 0)
		AfxThrowInvalidArgException();

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount /*=1*/)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if(nIndex < 0 || nCount <= 0)
		AfxThrowInvalidArgException();

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		INT_PTR nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		for( int i = 0; i < nCount; i++ )
			(m_pData + nOldSize + i)->~TYPE();
		// shift old data up to fill gap
		::ATL::Checked::memmove_s(m_pData + nIndex + nCount, (nOldSize-nIndex) * sizeof(TYPE),
			m_pData + nIndex, (nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		memset((void*)(m_pData + nIndex), 0, (size_t)nCount * sizeof(TYPE));
		for( int i = 0; i < nCount; i++ )
#pragma push_macro("new")
#undef new
			::new( (void*)( m_pData + nIndex + i ) ) TYPE;
#pragma pop_macro("new")
	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	INT_PTR nUpperBound = nIndex + nCount;
	ASSERT(nUpperBound <= m_nSize && nUpperBound >= nIndex && nUpperBound >= nCount);

	if(nIndex < 0 || nCount < 0 || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
		AfxThrowInvalidArgException();

	// just remove a range
	INT_PTR nMoveCount = m_nSize - (nUpperBound);
	for( int i = 0; i < nCount; i++ )
		(m_pData + nIndex + i)->~TYPE();
	if (nMoveCount)
	{
		::ATL::Checked::memmove_s(m_pData + nIndex, (size_t)nMoveCount * sizeof(TYPE),
			m_pData + nUpperBound, (size_t)nMoveCount * sizeof(TYPE));
	}
	m_nSize -= nCount;
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::InsertAt(INT_PTR nStartIndex, CArrayEx* pNewArray)
{
	ASSERT_VALID(this);
	ASSERT(pNewArray != NULL);
	ASSERT_VALID(pNewArray);
	ASSERT(nStartIndex >= 0);

	if(pNewArray == NULL || nStartIndex < 0)
		AfxThrowInvalidArgException();

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (INT_PTR i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

template<class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar.WriteCount(m_nSize);
	}
	else
	{
		DWORD_PTR nOldSize = ar.ReadCount();
		SetSize(nOldSize, -1);
	}
	SerializeElements<TYPE>(ar, m_pData, m_nSize);
}

////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMapEx : public CObject
{
public:
	// CPair
	struct CPair
	{
		const KEY key;
		VALUE value;
	protected:
		CPair( ARG_KEY keyval ) : key( keyval )	{}
	};

protected:
	// Association
	class CAssoc : public CPair
	{
		friend class CMapEx<KEY,ARG_KEY,VALUE,ARG_VALUE>;
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
	public:
		CAssoc( ARG_KEY key ) : CPair( key ) {}
	};

public:
// Construction
	/* explicit */ CMapEx(INT_PTR nBlockSize = 10);

// Attributes
	// number of elements
	INT_PTR GetCount() const;
	INT_PTR GetSize() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;
	const CPair *PLookup(ARG_KEY key) const;
	CPair *PLookup(ARG_KEY key);

// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;

	const CPair *PGetFirstAssoc() const;
	CPair *PGetFirstAssoc();

	void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	const CPair *PGetNextAssoc(const CPair *pAssocRet) const;
	CPair *PGetNextAssoc(const CPair *pAssocRet);

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CPlexEx* m_pBlocks;
	INT_PTR m_nBlockSize;

	CAssoc* NewAssoc(ARG_KEY key);
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(ARG_KEY, UINT&, UINT&) const;

public:
	~CMapEx();
	void Serialize(CArchive&);
#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE> inline functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE INT_PTR CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
	{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE INT_PTR CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetSize() const
	{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE BOOL CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
	{ return m_nCount == 0; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
	{ (*this)[key] = newValue; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE POSITION CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc() const
{ 
	ASSERT_VALID(this);
	if(m_nCount == 0) return NULL;

	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)BEFORE_START_POSITION;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc()
{ 
	ASSERT_VALID(this);
	if(m_nCount == 0) return NULL;

	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)BEFORE_START_POSITION;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE UINT CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMapEx(INT_PTR nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		//delete[] m_pHashTable;
		DeleteMem( m_pHashTable );
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		//m_pHashTable = new CAssoc* [nHashSize];
		m_pHashTable = (CAssoc**)AllocMem(sizeof(CAssoc*) * nHashSize);
		ENSURE(m_pHashTable != NULL);
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT_VALID(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				pAssoc->CAssoc::~CAssoc();
				//DestructElements<VALUE>(&pAssoc->value, 1);
				//DestructElements<KEY>((KEY*)&pAssoc->key, 1);
			}
		}
	}

	// free hash table
	//delete[] m_pHashTable;
	DeleteMem( m_pHashTable );
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CMapEx()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc(ARG_KEY key)
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlexEx* newBlock = CPlexEx::Create(m_pBlocks, m_nBlockSize, sizeof(CMapEx::CAssoc));
		// chain them into free list
		CMapEx::CAssoc* pAssoc = (CMapEx::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (INT_PTR i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ENSURE(m_pFreeList != NULL);  // we must have something

	CMapEx::CAssoc* pAssoc = m_pFreeList;

	// zero the memory
	CMapEx::CAssoc* pTemp = pAssoc->pNext;
	memset( pAssoc, 0, sizeof(CMapEx::CAssoc) );
	pAssoc->pNext = pTemp;

	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
#pragma push_macro("new")
#undef new
	::new(pAssoc) CMapEx::CAssoc(key);
#pragma pop_macro("new")
//	ConstructElements<KEY>(&pAssoc->key, 1);
//	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(CAssoc* pAssoc)
{
	pAssoc->CAssoc::~CAssoc();
//	DestructElements<VALUE>(&pAssoc->value, 1);
//	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHashBucket, UINT& nHashValue) const
// find association (or return NULL)
{
	nHashValue = HashKey<ARG_KEY>(key);
	nHashBucket = nHashValue % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHashBucket]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->nHashValue == nHashValue && CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT_VALID(this);

	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key) const
{
	ASSERT_VALID(this);

	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key)
{
	ASSERT_VALID(this);

	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT_VALID(this);

	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHashBucket, nHashValue)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		ENSURE(m_pHashTable);
		// it doesn't exist, add a new Association
		pAssoc = NewAssoc(key);
		pAssoc->nHashValue = nHashValue;
		//'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHashBucket];
		m_pHashTable[nHashBucket] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	UINT nHashValue;
	CAssoc** ppAssocPrev;
	nHashValue = HashKey<ARG_KEY>(key);
	ppAssocPrev = &m_pHashTable[nHashValue%m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if ((pAssoc->nHashValue == nHashValue) && CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition,
	KEY& rKey, VALUE& rValue) const
{
	ASSERT_VALID(this);
	ENSURE(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ENSURE(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		{
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			{
				break;
			}
		}
		ENSURE(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair*
CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* pPairRet) const
{
	ASSERT_VALID(this);

	CAssoc* pAssocRet = (CAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);
	
	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;
		
	ASSERT(pAssocRet != (CAssoc*)BEFORE_START_POSITION);

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair*
CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* pPairRet)
{
	ASSERT_VALID(this);

	CAssoc* pAssocRet = (CAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);
	
	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;
		
	ASSERT(pAssocRet != (CAssoc*)BEFORE_START_POSITION);

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.WriteCount(m_nCount);
		if (m_nCount == 0)
			return;  // nothing more to do

		ASSERT(m_pHashTable != NULL);
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				KEY* pKey;
				VALUE* pValue;
				/* 
				 * in some cases the & operator might be overloaded, and we cannot use it to 
				 * obtain the address of a given object.  We then use the following trick to 
				 * get the address
				 */
				pKey = reinterpret_cast< KEY* >( &reinterpret_cast< int& >( const_cast< KEY& > ( static_cast< const KEY& >( pAssoc->key ) ) ) );
				pValue = reinterpret_cast< VALUE* >( &reinterpret_cast< int& >( static_cast< VALUE& >( pAssoc->value ) ) );
				SerializeElements<KEY>(ar, pKey, 1);
				SerializeElements<VALUE>(ar, pValue, 1);
			}
		}
	}
	else
	{
		DWORD_PTR nNewCount = ar.ReadCount();
		while (nNewCount--)
		{
			KEY newKey[1];
			VALUE newValue[1];
			SerializeElements<KEY>(ar, newKey, 1);
			SerializeElements<VALUE>(ar, newValue, 1);
			SetAt(newKey[0], newValue[0]);
		}
	}
}

#ifdef _DEBUG
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		KEY key[1];
		VALUE val[1];

		POSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			GetNextAssoc(pos, key[0], val[0]);
			dc << "\n\t[";
			DumpElements<KEY>(dc, key, 1);
			dc << "] = ";
			DumpElements<VALUE>(dc, val, 1);
		}
	}

	dc << "\n";
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////