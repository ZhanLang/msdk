#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include "RapidXMLWrapper.h"
#include "rapidxml.h"

#include "luahost/luahost.h"
using namespace std;

static int DOCUMENTS_INDEX(LUA_NOREF);
static int DOCUMENTS_REFCOUNT_INDEX(LUA_NOREF);


typedef struct CXmlDocumentWrapper_ud 
{
    CXmlDocumentWrapper *xmldoc;
} CXmlDocumentWrapper_ud;

typedef struct CXmlNodelistWrapper_ud 
{
    CXmlNodelistWrapper *xmlnode;
} CXmlNodelistWrapper_ud;

typedef struct CXmlNodeWrapper_ud 
{
    CXmlNodeWrapper *xmlnode;
} CXmlNodeWrapper_ud;


static void decrease_tixmldocument_refcount(lua_State *L, const rapidxml::xml_document<> *xmldoc) 
{
    long docptr(reinterpret_cast<long>(xmldoc));

    lua_rawgeti(L, LUA_REGISTRYINDEX, DOCUMENTS_REFCOUNT_INDEX);
    int refcount_table_index(lua_gettop(L));
    lua_pushinteger(L, docptr);
    lua_gettable(L, -2);

    if (lua_istable(L, -1)) 
    {
        lua_pushinteger(L, 2);
        lua_gettable(L, -2);

        int refcount(lua_tointeger(L, -1));
        lua_pop(L, 1);

        if (refcount == 1) 
        {
            // destroy table
            lua_pushinteger(L, docptr);
            lua_pushnil(L);
            lua_settable(L, refcount_table_index);
            //cout << "current refcount for " << xmldoc << " : 0" << endl;
        } 
        else 
        {
            lua_pushinteger(L, 2);
            lua_pushinteger(L, refcount-1);
            lua_settable(L, -3);
            //cout << "current refcount for " << xmldoc << " : " << refcount-1 << endl;
        }
    }

    lua_pop(L, 2);
}

static void increase_tixmldocument_refcount(lua_State *L, const rapidxml::xml_document<>* xmldoc) 
{
    // increase ref. count for corresponding document in DOCUMENTS table
    long docptr(reinterpret_cast<long>(xmldoc));

    // 1) get document's userdata
    lua_rawgeti(L, LUA_REGISTRYINDEX, DOCUMENTS_INDEX);
    lua_pushinteger(L, docptr);
    lua_gettable(L, -2);
    CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) lua_touserdata(L, -1);
    int ud_index(lua_gettop(L));

    if (xmldoc_userdata) 
    {
        // 2) get refcount table
        lua_rawgeti(L, LUA_REGISTRYINDEX, DOCUMENTS_REFCOUNT_INDEX);
        lua_pushinteger(L, docptr);
        lua_gettable(L, -2);

        if (lua_istable(L, -1)) 
        {
            // 3) increase refcount ...
            lua_pushinteger(L, 2);
            lua_gettable(L, -2);
            int refcount(lua_tointeger(L, -1));
            lua_pop(L, 1);

            lua_pushinteger(L, 2);
            lua_pushinteger(L, refcount + 1);
            lua_settable(L, -3);

            //cout << "current refcount for " << xmldoc << " : " << refcount+1 << endl;
        } 
        else 
        {
            // ... or create new refcount table
            lua_pushinteger(L, docptr);

            lua_createtable(L, 2, 0);
            lua_pushinteger(L, 1);
            lua_pushvalue(L, ud_index); //create a strong reference to the document's ud
            lua_settable(L, -3);
            lua_pushinteger(L, 2);
            lua_pushinteger(L, 1);
            lua_settable(L, -3);

            lua_settable(L, ud_index+1);

            //cout << "current refcount for " << xmldoc << " : 1" << endl;
        }

        lua_pop(L, 2);
    }

    lua_pop(L, 2);
}

extern "C"{

    static char * getVersion( )
    {
        static char marker[] = { LIBNAME " v" LIBVER " (c) DForD Software."};
        return( marker );
    }

    static int xmlopen(lua_State* L) 
    {
        const char *filename = luaL_checkstring(L, 1);
        CXmlDocumentWrapper_ud *xpu;

        lua_pop(L, 1);

        xpu = (CXmlDocumentWrapper_ud *) lua_newuserdata(L, sizeof(CXmlDocumentWrapper_ud));
        // set metatable for userdata
        luaL_getmetatable(L, "CXmlDocument");
        lua_setmetatable(L, -2);
		
		CXmlDocumentWrapper *xmldoc = new CXmlDocumentWrapper;
		xpu->xmldoc = xmldoc;
		bool loadOkay = false;
		try {
			loadOkay = xmldoc->Load(filename);
		} catch (std::exception&) {
			/* do nothing */
		}

		if ( !loadOkay ) {
            //return luaL_error(L, "error: could not load file %s", filename); 
			lua_pushnil(L);
			return 0;
		}

        // put document into DOCUMENTS_INDEX table
        lua_rawgeti(L, LUA_REGISTRYINDEX, DOCUMENTS_INDEX);
        lua_pushinteger(L, reinterpret_cast<long>(xmldoc->Interface()));
        lua_pushvalue(L, -3);
        lua_settable(L, -3);
        lua_pop(L, 1);

        return 1;
    }

    static int xmlparse(lua_State* L) 
    {
        const char* xml = luaL_checkstring(L, 1);
        CXmlDocumentWrapper_ud *xpu;

        lua_pop(L, 1);

        xpu = (CXmlDocumentWrapper_ud *) lua_newuserdata(L, sizeof(CXmlDocumentWrapper_ud));
        // set metatable for userdata
        luaL_getmetatable(L, "CXmlDocument");
        lua_setmetatable(L, -2);

		CXmlDocumentWrapper *xmldoc = new CXmlDocumentWrapper;
		xpu->xmldoc = xmldoc;
		xmldoc->LoadXML((char *)xml);

        // put document into DOCUMENTS_INDEX table
        lua_rawgeti(L, LUA_REGISTRYINDEX, DOCUMENTS_INDEX);
        lua_pushinteger(L, reinterpret_cast<long>(xmldoc->Interface()));
        lua_pushvalue(L, -3);
        lua_settable(L, -3);
        lua_pop(L, 1);

        return 1;
    }

    static const struct luaL_reg rapidxmlCWapper[] = {
        { "open", xmlopen},
        { "parse", xmlparse},
        {NULL, NULL} /* sentinel */
    };

    static int CXmlDocumentWrapper_close(lua_State* L)
    {
        CXmlDocumentWrapper *xmldoc;
        CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) luaL_checkudata(L, 1, "CXmlDocument");
        xmldoc = xmldoc_userdata->xmldoc;

        lua_pop(L, 1);

        // free CXmlDocumentWrapper instance if there is one
        if (xmldoc) 
        {
            delete xmldoc;
            xmldoc_userdata->xmldoc = NULL;
        }

        return 0;
    }

    static int CXmlDocumentWrapper_repr(lua_State* L)
    {
        CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) luaL_checkudata(L, 1, "CXmlDocument");

        lua_pop(L, 1);

        lua_pushfstring(L, "<xml document at: '%p`>", xmldoc_userdata->xmldoc);

        return 1;
    }

    static int CXmlDocumentWrapper_xml(lua_State* L)
    {
        CXmlDocumentWrapper *xmldoc;
        CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) luaL_checkudata(L, 1, "CXmlDocument");
        xmldoc = xmldoc_userdata->xmldoc;

        string s = xmldoc->GetXML();
        lua_pushstring(L, s.c_str());
        return 1;
    }

    static int CXmlDocumentWrapper_save(lua_State* L)
    {
        CXmlDocumentWrapper *xmldoc;
        CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) luaL_checkudata(L, 1, "CXmlDocument");
        xmldoc = xmldoc_userdata->xmldoc;
        const char *filename = luaL_checkstring(L, 2);
        bool bPrettyPrint = false;
        if (lua_isboolean(L, 3))
        {
            bPrettyPrint = !!lua_toboolean(L, 3);
        }
        else
        {
            lua_pop(L, 2);
            return luaL_typerror(L, 1, "boolean");
        }

        lua_pop(L, 3);

		//std::string sXml = xmldoc->GetXML();

        if (xmldoc->Save(filename, bPrettyPrint)) 
        {
            lua_pushboolean(L, 1);
            return 1;
        } 
        else 
        {
            //return luaL_error(L, "could not save xml to %s", filename);
			lua_pushboolean(L, 0);
            return 1;
        }
    }

	// add jilei 2013-2-25
	static int CXmlDocumentWrapper_saveutf8(lua_State* L)
	{
		CXmlDocumentWrapper *xmldoc;
		CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) luaL_checkudata(L, 1, "CXmlDocument");
		xmldoc = xmldoc_userdata->xmldoc;
		const char *filename = luaL_checkstring(L, 2);
		bool bPrettyPrint = false;
		if (lua_isboolean(L, 3))
		{
			bPrettyPrint = !!lua_toboolean(L, 3);
		}
		else
		{
			lua_pop(L, 2);
			return luaL_typerror(L, 1, "boolean");
		}

		lua_pop(L, 3);

		if (xmldoc->SaveUtf8(filename, bPrettyPrint)) 
		{
			lua_pushboolean(L, 1);
			return 1;
		} 
		else 
		{
			//return luaL_error(L, "could not save xml to %s", filename);
			lua_pushboolean(L, 0);
			return 1;
		}
	}

    static int CXmlDocumentWrapper_root(lua_State *L)
    {
        CXmlDocumentWrapper_ud* xmldoc_userdata = (CXmlDocumentWrapper_ud *) luaL_checkudata(L, 1, "CXmlDocument");
        CXmlDocumentWrapper *xmldoc = xmldoc_userdata->xmldoc;

        lua_pop(L, 1);

        CXmlNodeWrapper_ud* node_userdata = (CXmlNodeWrapper_ud *) lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
        node_userdata->xmlnode = new CXmlNodeWrapper(xmldoc->/*AsNode*/AsDocument()); //->Clone();

        luaL_newmetatable(L, "CXmlNode");
        lua_setmetatable(L, -2);

		xmldoc->GetXML();

        increase_tixmldocument_refcount(L, xmldoc->Interface());

        return 1;
    }


    static const struct luaL_reg CXmlDocumentWrapper_methods[] = {
        { "__gc", CXmlDocumentWrapper_close },
        { "__tostring", CXmlDocumentWrapper_repr },
        { "save", CXmlDocumentWrapper_save },
        { "root", CXmlDocumentWrapper_root},
        { "xml", CXmlDocumentWrapper_xml },
		{ "saveutf8", CXmlDocumentWrapper_saveutf8 },
        { NULL, NULL }
    };

    static int CXmlNode_repr(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);

        if (xmlnode && xmlnode->IsValid()) 
        {
            lua_pushfstring(L, "< %s node>", xmlnode->Name().c_str());
        } 
        else 
        {
            return luaL_error(L, "invalid node");
        }

        return 1;
    }

    static int CXmlNode_close(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);

        // decrease refcount for corresponding document
        if (xmlnode && xmlnode->IsValid())
            decrease_tixmldocument_refcount(L, xmlnode->Document()); // ?? 

        delete xmlnode;
        xmlnode_userdata->xmlnode = NULL;

        return 0;
    }

    static int CXmlNode_valid(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);

        if (xmlnode)
            lua_pushboolean(L, xmlnode->IsValid());
        else 
            lua_pushboolean(L, false );

        return 1;
    }

    static int CXmlNode_type(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);

        if (xmlnode && xmlnode->IsValid())
        {
            lua_pushinteger(L, xmlnode->GetNodeType());
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }        

        return 1;
    }

    static int CXmlNode_exists_attr(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        const char *attribute = luaL_checkstring(L, 2); // attribute

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            lua_pushboolean(L, xmlnode->ExistAttribute(attribute));
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }
        
        return 1;
    }

    static int CXmlNode_set_attr(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        const char *attribute = luaL_checkstring(L, 2); // path
        const char *attr_value = luaL_checkstring(L, 3); // attribute value

        lua_pop(L, 3);
        if (xmlnode && xmlnode->IsValid())
        {
            xmlnode->SetValue(attribute, attr_value);
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }        

        return 1;
    }

    static int CXmlNode_attribute(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        const char *attribute = luaL_checkstring(L, 2); // attribute

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            lua_pushstring(L, xmlnode->GetValue(attribute).c_str());
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }

        return 1;
    }

    static int CXmlNode_name(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);
        if (xmlnode && xmlnode->IsValid())
        {
            lua_pushstring(L, xmlnode->Name().c_str());
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }

        return 1;
    }

    static int CXmlNode_text(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);
        if (xmlnode && xmlnode->IsValid())
        {
            std::string text = xmlnode->GetText();
            if (text.empty())
            {
                rapidxml::xml_node<>* node = xmlnode->Interface();
                node = node->first_node();
                if (node && node->type() == rapidxml::node_cdata)
                {
                    lua_pushstring(L, node->value());
                }
                else
                {
                    lua_pushstring(L, "");
                }
            }
            else
            {
                lua_pushstring(L, text.c_str());
            }
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }
        return 1;
    }

    static int CXmlNode_set_text(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        std::string text = luaL_checkstring(L, 2); // text

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            xmlnode->SetText(text.c_str());
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }  
		//xmlnode->Document()->
        return 1;
    }

    static int CXmlNode_parent(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);
        if (xmlnode && xmlnode->IsValid())
        {
            if (xmlnode->Parent())
            {
                CXmlNodeWrapper_ud* node_userdata = (CXmlNodeWrapper_ud *) lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
                node_userdata->xmlnode = new CXmlNodeWrapper(xmlnode->Parent()); //->Clone();

                luaL_newmetatable(L, "CXmlNode");
                lua_setmetatable(L, -2);

                increase_tixmldocument_refcount(L, node_userdata->xmlnode->Document());
            }
            else
            {
                lua_pushnil(L);
            }
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }    
        return 1;
    }

    static int CXmlNode_children(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 1);
        if (xmlnode && xmlnode->IsValid())
        {
            CXmlNodelistWrapper nodelist = xmlnode->ChildNodes();

            if (nodelist.empty())
            {
                //lua_pushnil(L);
                lua_newtable (L);
            }
            else
            {
                lua_newtable (L);
                int index = 0;
                for (size_t nIndex = 0; nIndex < nodelist.size(); nIndex++)
                {
                    lua_pushnumber (L,++index);
                    CXmlNodeWrapper_ud* xmlnode_ud = (CXmlNodeWrapper_ud*)lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
                    xmlnode_ud->xmlnode = new CXmlNodeWrapper(nodelist.at(nIndex));
                    luaL_newmetatable(L, "CXmlNode");
                    lua_setmetatable(L, -2);
                    lua_rawset (L, -3);
                    increase_tixmldocument_refcount(L, xmlnode_ud->xmlnode->Document());
                }  
            }        
        } 
        else
        {
            return luaL_error(L, "invalid node");
        }  
        return 1;
    }

    static int CXmlNode_remove(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        CXmlNodeWrapper_ud* xmlnoderemove_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 2, "CXmlNode");
        CXmlNodeWrapper *xmlnoderemove = xmlnoderemove_userdata->xmlnode;

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid() && xmlnoderemove && xmlnoderemove->IsValid())
        {
            xmlnode->RemoveNode(xmlnoderemove->Interface());
            decrease_tixmldocument_refcount(L, xmlnoderemove->Document());
            delete xmlnoderemove;
            xmlnoderemove_userdata->xmlnode = NULL;
        }
        else
        {
            return luaL_error(L, "invalid node");
        } 
        
        return 1;
    }

    static int CXmlNode_append(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
		if ( lua_isstring( L, 2 ) )
		{
        const char* nodename = luaL_checkstring(L, 2);

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            CXmlNodeWrapper_ud* xmlnode_ud = (CXmlNodeWrapper_ud*)lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
            xmlnode_ud->xmlnode = new CXmlNodeWrapper(xmlnode->InsertNode(-1, nodename));
            luaL_newmetatable(L, "CXmlNode");
            lua_setmetatable(L, -2);
            increase_tixmldocument_refcount(L, xmlnode_ud->xmlnode->Document());
        }
        else
        {
            return luaL_error(L, "invalid node");
        }        
		}
		else if ( lua_isuserdata( L, 2 ))
		{
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 2, "CXmlNode");
        CXmlNodeWrapper *xmlsubnode = xmlnode_userdata->xmlnode;

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            CXmlNodeWrapper_ud* xmlnode_ud = (CXmlNodeWrapper_ud*)lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
            xmlnode_ud->xmlnode = new CXmlNodeWrapper(xmlnode->InsertNode(-1, xmlsubnode->Interface()));
            luaL_newmetatable(L, "CXmlNode");
            lua_setmetatable(L, -2);
            increase_tixmldocument_refcount(L, xmlnode_ud->xmlnode->Document());
        }
        else
        {
            return luaL_error(L, "invalid node");
        }        
		}
        else
        {
            return luaL_error(L, "invalid node");
        }       


        return 1;
    }

    static int CXmlNode_find(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        const char* nodename = luaL_checkstring(L, 2);

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            rapidxml::xml_node<>* pNode = xmlnode->FindNode(nodename);
            if (pNode)
            {
                CXmlNodeWrapper_ud* xmlnode_ud = (CXmlNodeWrapper_ud*)lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
                xmlnode_ud->xmlnode = new CXmlNodeWrapper(pNode);
                luaL_newmetatable(L, "CXmlNode");
                lua_setmetatable(L, -2);
                increase_tixmldocument_refcount(L, xmlnode_ud->xmlnode->Document());
            }
            else
            {
                lua_pushnil(L);
            }
        }
        else
        {
            return luaL_error(L, "invalid node");
        }  

        return 1;
    }

    static int CXmlNode_select(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        const char* nodename = luaL_checkstring(L, 2);

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            CXmlNodelistWrapper nodelist = xmlnode->ChildNodes();
            lua_newtable (L);
            if (!nodelist.empty())
            {
                int index = 0;
                for (size_t nIndex = 0; nIndex < nodelist.size(); nIndex++)
                {
                    CXmlNodeWrapper node = nodelist.at(nIndex);
                    if (node.GetNodeType() == rapidxml::node_element
                        && node.Name() == nodename)
                    {
                        lua_pushnumber (L,++index);
                        CXmlNodeWrapper_ud* xmlnode_ud = (CXmlNodeWrapper_ud*)lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
                        xmlnode_ud->xmlnode = new CXmlNodeWrapper(nodelist.at(nIndex));
                        luaL_newmetatable(L, "CXmlNode");
                        lua_setmetatable(L, -2);
                        lua_rawset (L, -3);
                        increase_tixmldocument_refcount(L, xmlnode_ud->xmlnode->Document());
                    }
                }  
            }
        }
        else
        {
            return luaL_error(L, "invalid node");
        }  

        return 1;
    }

    static int CXmlNode_selectall(lua_State *L)
    {
        CXmlNodeWrapper_ud* xmlnode_userdata = (CXmlNodeWrapper_ud *) luaL_checkudata(L, 1, "CXmlNode");
        CXmlNodeWrapper *xmlnode = xmlnode_userdata->xmlnode;
        const char* nodename = luaL_checkstring(L, 2);

        lua_pop(L, 2);
        if (xmlnode && xmlnode->IsValid())
        {
            CXmlNodelistWrapper nodelist = xmlnode->FindNodes(nodename);
            lua_newtable (L);
            if (!nodelist.empty())
            {
                int index = 0;
                for (size_t nIndex = 0; nIndex < nodelist.size(); nIndex++)
                {
                    lua_pushnumber (L,++index);
                    CXmlNodeWrapper_ud* xmlnode_ud = (CXmlNodeWrapper_ud*)lua_newuserdata(L, sizeof(CXmlNodeWrapper_ud));
                    xmlnode_ud->xmlnode = new CXmlNodeWrapper(nodelist.at(nIndex));
                    luaL_newmetatable(L, "CXmlNode");
                    lua_setmetatable(L, -2);
                    lua_rawset (L, -3);
                    increase_tixmldocument_refcount(L, xmlnode_ud->xmlnode->Document());
                }  
            }  
        }
        else
        {
            return luaL_error(L, "invalid node");
        }  

        return 1;
    }

    static const struct luaL_reg CXmlNodeWrapper_methods[] = {
        { "__tostring", CXmlNode_repr },
        { "__gc", CXmlNode_close },
        { "valid", CXmlNode_valid },
        { "type", CXmlNode_type },
        { "exists_attr", CXmlNode_exists_attr },
        { "setattr", CXmlNode_set_attr },
        { "attribute", CXmlNode_attribute },
        { "name", CXmlNode_name },
        { "text", CXmlNode_text },
        { "settext", CXmlNode_set_text },
        { "parent", CXmlNode_parent },
        { "children", CXmlNode_children },
        { "remove", CXmlNode_remove },
        { "append", CXmlNode_append },
        { "find", CXmlNode_find },
        { "select", CXmlNode_select },
        { "select_all", CXmlNode_selectall },
        { NULL, NULL }
    };


    static const struct luaL_reg CXmlNodelistWrapper_methods[] = {
        { NULL, NULL }
    };

    int luaopen_rapidxml(void* _L) 
    {
		lua_State* L = (lua_State*)_L;
        // create metatable
        luaL_newmetatable(L, "CXmlDocument");
        // metatable.__index = metatable
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        // register methods
        luaL_register(L, NULL, CXmlDocumentWrapper_methods);

        luaL_newmetatable(L, "CXmlNodelist");
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_register(L, NULL, CXmlNodelistWrapper_methods);

        luaL_newmetatable(L, "CXmlNode");
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_register(L, NULL, CXmlNodeWrapper_methods);

        // create registry table for documents
        // 1) create documents table
        lua_createtable(L, 0, 10); // preallocate 10 key-value pairs
        // 2) create metatable for DOCUMENTS_INDEX
        //    DOCUMENTS_INDEX will be a weak table
        lua_createtable(L, 0, 1);
        lua_pushstring(L, "v");
        lua_setfield(L, -2, "__mode");
        // 3) set metatable
        lua_setmetatable(L, -2);
        DOCUMENTS_INDEX = luaL_ref(L, LUA_REGISTRYINDEX);

        lua_createtable(L, 0, 10);
        DOCUMENTS_REFCOUNT_INDEX = luaL_ref(L, LUA_REGISTRYINDEX);

        // register functions
        luaL_register(L, LIBNAME, rapidxmlCWapper);
        lua_getglobal(L, LIBNAME);

        char *version = getVersion();
        lua_pushliteral (L, "_VERSION");
        lua_pushstring (L, version );
        lua_rawset(L, -3);
        lua_pushstring(L, "node_document");
        lua_pushnumber(L,(lua_Number)rapidxml::node_document);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_element");
        lua_pushnumber(L,(lua_Number)rapidxml::node_element);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_data");
        lua_pushnumber(L,(lua_Number)rapidxml::node_data);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_cdata");
        lua_pushnumber(L,(lua_Number)rapidxml::node_cdata);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_comment");
        lua_pushnumber(L,(lua_Number)rapidxml::node_comment);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_declaration");
        lua_pushnumber(L,(lua_Number)rapidxml::node_declaration);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_doctype");
        lua_pushnumber(L,(lua_Number)rapidxml::node_doctype);
        lua_rawset(L,-3);
        lua_pushstring(L, "node_pi");
        lua_pushnumber(L,(lua_Number)rapidxml::node_pi);
        lua_rawset(L,-3);

        lua_pop(L, 1);
        return 1;
    }
};