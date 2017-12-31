/**
 * @file templateprioritymanager.h
 * @brief Simple chained list with uses of priority for orders element.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-02-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_TEMPLATEPRIORITYMANAGER_H
#define _O3D_TEMPLATEPRIORITYMANAGER_H

#include "memorydbg.h"
#include "baseobject.h"

namespace o3d {

//---------------------------------------------------------------------------------------
//! @class PriorityManagerElt
//-------------------------------------------------------------------------------------
//! Element of O3DTemplatePriorityManager. Contain, his son and his priority.
//---------------------------------------------------------------------------------------
template<class T>
class O3D_API_TEMPLATE PriorityManagerElt
{
public:

	T* Element;                 //!< data.
	PriorityManagerElt* Son;    //!< next item (less priority).
	Int32 Priority;
	Int32 Number;           //!< element id.

	//!  default constructor
	PriorityManagerElt()
	{
        Element = nullptr;
        Son = nullptr;
		Priority = 0;
		Number = 0;
	}

	PriorityManagerElt(
			T* element,
			PriorityManagerElt<T> *son,
			Int32 priority,
			Int32 number)
	{
		Element = element;
		Son = son;
		Priority = priority;
		Number = number;
	}

    PriorityManagerElt(T *element)
	{
    	Element = element;
        Son = nullptr;
		Priority = 0;
		Number = 0;
	}

	virtual ~PriorityManagerElt()
	{
		deletePtr(Son);
		deletePtr(Element);
	}
};

//---------------------------------------------------------------------------------------
//! @class TemplatePriorityManager
//-------------------------------------------------------------------------------------
//! Simple chained list with uses of priority for orders element.
//---------------------------------------------------------------------------------------
template<class T>
class O3D_API_TEMPLATE TemplatePriorityManager : NonCopyable<>
{
public:

	//! default constructor
	TemplatePriorityManager()
	{
		m_NextId = 0;
		m_size = 0;
        m_Root = nullptr;
        m_Selected = nullptr;
	}

	virtual ~TemplatePriorityManager() { destroy(); }

	//! delete all elements
	void destroy()
	{
		deletePtr(m_Root);

        m_Selected = nullptr;
		m_size = 0;
		m_NextId = 0;
	}

	//! Get the root element.
	PriorityManagerElt<T>* getRoot() { return m_Root; }

	//! add new element with priority. return its index
	Int32 addElement(T* t, Int32 priority)
	{
        if (m_Root != nullptr)
		{
			PriorityManagerElt<T> *elt = m_Root;
			PriorityManagerElt<T> *eltFather = m_Root;

			// find an element with a superior priority (0 = first, 1 =second, ...) and use it as father
            while (elt != nullptr && elt->Priority < priority)
			{
				eltFather = elt;
				elt = elt->Son;
			}

			if (elt != m_Root)// && priority >= elt_father->m_Priority)
			{
				PriorityManagerElt<T> *new_elt = new PriorityManagerElt<T>(t,elt,priority,m_NextId);
				m_Selected = new_elt;
				eltFather->Son = new_elt;
			}
			else// if (priority >= elt_father->m_Priority)
			{
				m_Root = new PriorityManagerElt<T>(t,m_Root,priority,m_NextId);
				m_Selected = m_Root;
			}
			//else
			//{
            //	O3DPriorityManagerElt<T> *new_elt = new O3DPriorityManagerElt<T>(t,nullptr,priority,m_NextId);
			//	elt_father->m_Son = new_elt;
			//	m_Selected = new_elt;
			//}

			++m_NextId;
			++m_size;
			return m_NextId-1;
		}
		else
		{
            m_Root = new PriorityManagerElt<T>(t,nullptr,priority,m_NextId);
			m_Selected = m_Root;
			++m_NextId;
			++m_size;
			return m_Root->Number;
		}
	}

	//! delete element by its index
	Bool deleteElement(Int32 index)
	{
		PriorityManagerElt<T> *elt = m_Root;
		PriorityManagerElt<T> *eltFather = m_Root;

		// find element
        while (elt != nullptr && elt->Number != index)
		{
			eltFather = elt;
			elt = elt->Son;
		}

        if (elt != nullptr && elt->Number == index)
		{
			// ordering
			if (eltFather != elt)
				eltFather->Son = elt->Son;
			else
				m_Root = elt->Son;

			// delete item
            elt->Son = nullptr;
			deletePtr(elt);
			--m_size;

			return True;
		}
		return False;
	}

	//! delete selected element, and selected element will be Root
	Bool deleteElement()
	{
		PriorityManagerElt<T> *elt = m_Root;
		PriorityManagerElt<T> *eltFather = m_Root;

        while (elt != nullptr && elt != m_Selected)
		{
			eltFather = elt;
			elt = elt->Son;
		}

        if (elt != nullptr && elt == m_Selected)
		{
			// reset sons
			if (eltFather != elt)
				eltFather->Son = elt->Son;
			else
				m_Root = elt->Son;

			// delete item data
            elt->Son = nullptr;
			deletePtr(elt);
			--m_size;

			m_Selected = m_Root;
			return True;
		}
        return False;
	}

	//! get element by its index
	T* find(Int32 index)
	{
		PriorityManagerElt<T> *elt = m_Root;
        while (elt != nullptr && elt->Number != index) elt = elt->Son;
		if (elt->Number == index) return elt->Element;
        return nullptr;
	}
	const T* find(Int32 index)const
	{
		PriorityManagerElt<T> *elt = m_Root;
        while (elt != nullptr && elt->Number != index) elt = elt->Son;
		if (elt->Number == index) return elt->Element;
        return nullptr;
	}

	//! select an element by its index
	Int32 select(Int32 index)
	{
		PriorityManagerElt<T> *elt = m_Root;
        while (elt != nullptr && elt->Number != index) elt = elt->Son;
		if (elt->Number == index) m_Selected = elt;
		return m_Selected->Number;
	}

	//! select next element and return its index
	Int32 selectNext()
	{
        if (m_Selected == nullptr)
		{
			m_Selected = m_Root;
            if (m_Root == nullptr) return -1;
		}

		m_Selected = m_Selected->Son;
        if (m_Selected == nullptr) m_Selected = m_Root;
		return m_Selected->Number;
	}

	//! select root
	inline Int32 selectRoot()
	{
		m_Selected = m_Root;
		return m_Selected->Number;
	}

	//! get elements numbers
	inline Int32 getSize()const  { return m_size; }
	inline Int32 getNumElt()const { return m_size; }

	//! is the manager empty
	inline Bool isEmpty()const { return (m_size==0); }

	//! return the selected index
	inline Int32 getSelectedIndex()const
	{
        if (m_Selected == nullptr)
			return -1;
		else
			return m_Selected->Number;
	}

	//! get selected item data
	inline       T* getSelected()       { return m_Selected->Element; }
	//! get selected item data (read only).
	inline const T* getSelected() const { return m_Selected->Element; }

	//! change priority for select element, and return its index
	Int32 changePriority(Int32 priority)
	{
		PriorityManagerElt<T> *elt = m_Root;
		PriorityManagerElt<T> *eltFather = m_Root;

		// find selected item and his father
        while (elt != nullptr && elt != m_Selected)
		{
			eltFather = elt;
			elt = elt->Son;
		}

		// do with
        if (elt != nullptr && elt == m_Selected)
		{
			elt->Priority = priority;

			// reset sons
			if (eltFather != elt)
				eltFather->Son = elt->Son;
			else
				m_Root = elt->Son;

			// reset elements
			eltFather = m_Root;
            while (eltFather != nullptr && eltFather->Priority > priority)
				eltFather = eltFather->Son;

			elt->Son = eltFather->Son;
			eltFather->Son = elt;
		}
		return m_Selected->Number;
	}

protected:

	Int32 m_NextId;                     //!< id for next element.

	PriorityManagerElt<T> *m_Root;       //!< tree root.
	PriorityManagerElt<T> *m_Selected;   //!< selected element.

	Int32 m_size;                       //!< elements numbers.
};

} // namespace o3d

#endif // _O3D_TEMPLATEPRIORITYMANAGER_H

