/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#ifndef __FSLIST_H_
#define __FSLIST_H_

namespace fs
{

template <class T>
class list
{

public:
	template <class T>
	struct ListItem 
	{
		T data;
		ListItem *next, *prev;
	};

	void lock ()
	{
		EnterCriticalSection (&m_cs);
	}

	void unlock ()
	{
		LeaveCriticalSection (&m_cs);
	}

	BOOL save (HANDLE hFile)
	{
		lock ();

		int sz = size ();
		DWORD dw;

		if (!WriteFile (hFile, &sz, sizeof (sz), &dw, NULL))
		{
			unlock ();
			return FALSE;
		}

		for (int i = 0; i < sz; i++)
		{
			if (!WriteFile (hFile, &at (i), sizeof (T), &dw, NULL))
			{
				unlock ();
				return FALSE;
			}
		}

		unlock ();
		return TRUE;
	}

	BOOL load (HANDLE hFile)
	{
		lock ();

		int sz;
		DWORD dw;

		if (!ReadFile (hFile, &sz, sizeof (sz), &dw, NULL) || dw != sizeof (sz))
		{
			unlock ();
			return FALSE;
		}

		if (sz < 0)
		{
			unlock ();
			return FALSE;
		}

		while (sz--)
		{
			T data;
			if (!ReadFile (hFile, &data, sizeof (T), &dw, NULL) || dw != sizeof (T))
			{
				unlock ();
				return FALSE;
			}

			add (data);
		}

		unlock ();
		return TRUE;
	}

	void add (T data)
	{
		lock ();

		ListItem <T> *item = new ListItem <T>;

		item->data = data;
		item->next = item->prev = NULL;

		if (end)
		{
			end->next = item;
			item->prev = end;
			end = item;
		}
		else
		{
			start = end = item;
		}

		_size++;

		unlock ();
	}

	void insert (int iInsert, T data)
	{
		lock ();

		ListItem <T> *itemAfter = start;
		ListItem <T> *itemBefore = start;
		ListItem <T> *item;

		last = NULL;

		while (iInsert && itemAfter)
		{
			itemBefore = itemAfter;
			itemAfter = itemAfter->next;
			iInsert--;
		}

		if (itemAfter == NULL)
		{
			add (data);
			unlock ();
			return;
		}

		item = new ListItem <T>;

		item->data = data;
		item->next = itemAfter;
		item->prev = itemBefore;

		itemAfter->prev = item;

		if (itemAfter == itemBefore)
		{
			start = item;
			item->prev = NULL;
		}
		else
		{
			itemBefore->next = item;
		}

		_size++;
		unlock ();
	}

	void del (int i)
	{
		lock ();

		ListItem <T> *item = start;

		while (i--)
		{
			item = item->next;
		}

		if (item->prev)
			item->prev->next = item->next;
		else
			start = item->next;

		if (item->next)
			item->next->prev = item->prev;
		else
			end = item->prev;

		delete item;

		last = NULL;

		_size--;

		unlock ();
	}

	void clear ()
	{
		lock ();

		if (!start)
		{
			unlock ();
			return;
		}

		ListItem <T> *item;

		while (start)
		{
			item = start->next;
			delete start;
			start = item;
		}

		start = end = NULL;
		last = NULL;
		_size = 0;

		unlock ();
	}

	int size () const
	{
		return _size;
	}

	T& at (int i)
	{
		lock ();

		ListItem <T> *item = NULL;

		if (last)
		{
			if (lastIndex == i)
				item = last;
			else if (i == lastIndex - 1)
			{
				item = last->prev;
			}
			else if (i == lastIndex+1)
			{
				item = last->next;
			}
		}

		lastIndex = i;
		
		if (item == NULL)
		{
			item = start;
			while (i--) 
				item = item->next;
		}

		last = item;

		unlock ();
		
		return item->data;
	}

	int find (T& item)
	{
		lock ();
		for (int i = 0; i < size (); i++)
			if (at (i) == item)
				break;

		unlock ();
		return i != size () ? i : -1;
	}

	T& operator [] (int i)
	{
		return at (i);		
	}

	list& operator = (list& l)
	{
		lock ();

		clear ();
		for (int i = 0; i < l.size (); i++)
			add (l [i]);

		unlock ();

		return *this;
	}

	list (list& l)
	{
		start = end = NULL;
		last = NULL;
		_size = 0;

		InitializeCriticalSection (&m_cs);

		*this = l;
	}

	list () 
	{ 
		start = end = NULL;
		last = NULL;
		_size = 0;

		InitializeCriticalSection (&m_cs);
	}

	~list () 
	{
		clear ();
		DeleteCriticalSection (&m_cs);
	};

private:
	ListItem <T> *start, *end;
	ListItem <T> *last; int lastIndex;
	int _size;
	CRITICAL_SECTION m_cs;
};

};

#endif