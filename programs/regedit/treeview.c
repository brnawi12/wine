/*
 * Regedit treeview
 *
 * Copyright (C) 2002 Robert Dickenson <robd@reactos.org>
 * Copyright (C) 2008 Alexander N. Sørnes <alex@thehandofagony.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define WIN32_LEAN_AND_MEAN     /* Exclude rarely-used stuff from Windows headers */

#define NONAMELESSUNION

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <wine/debug.h>
#include <shlwapi.h>

#include "main.h"
#include "regproc.h"

WINE_DEFAULT_DEBUG_CHANNEL(regedit);

/* Global variables and constants  */
/* Image_Open, Image_Closed, and Image_Root - integer variables for indexes of the images.  */
/* CX_BITMAP and CY_BITMAP - width and height of an icon.  */
/* NUM_BITMAPS - number of bitmaps to add to the image list.  */
int Image_Open;
int Image_Closed;
int Image_Root;

#define CX_ICON    16
#define CY_ICON    16
#define NUM_ICONS    3

static BOOL UpdateExpandingTree(HWND hwndTV, HTREEITEM hItem, int state);

static BOOL get_item_path(HWND hwndTV, HTREEITEM hItem, HKEY* phKey, LPWSTR* pKeyPath, int* pPathLen, int* pMaxChars)
{
    TVITEMW item;
    int maxChars, chars;
    LPWSTR newStr;
    HTREEITEM hParent;

    item.mask = TVIF_PARAM;
    item.hItem = hItem;
    if (!TreeView_GetItemW(hwndTV, &item)) return FALSE;

    if (item.lParam) {
    /* found root key with valid key value */
    *phKey = (HKEY)item.lParam;
    return TRUE;
    }

    hParent = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
    if(!get_item_path(hwndTV, hParent, phKey, pKeyPath, pPathLen, pMaxChars)) return FALSE;
    if (*pPathLen) {
        (*pKeyPath)[*pPathLen] = '\\';
        ++(*pPathLen);
    }

    do {
        item.mask = TVIF_TEXT;
        item.hItem = hItem;
        item.pszText = *pKeyPath + *pPathLen;
        item.cchTextMax = maxChars = *pMaxChars - *pPathLen;
        if (!TreeView_GetItemW(hwndTV, &item)) return FALSE;
        chars = lstrlenW(item.pszText);
    if (chars < maxChars - 1) {
            *pPathLen += chars;
            break;
    }
    newStr = HeapReAlloc(GetProcessHeap(), 0, *pKeyPath, *pMaxChars * 2);
    if (!newStr) return FALSE;
    *pKeyPath = newStr;
    *pMaxChars *= 2;
    } while(TRUE);

    return TRUE;
}

LPWSTR GetItemPath(HWND hwndTV, HTREEITEM hItem, HKEY* phRootKey)
{
    int pathLen = 0, maxLen = 1024;
    WCHAR *pathBuffer;

    if (!hItem) {
         hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CARET, 0);
         if (!hItem) return NULL;
    }

    pathBuffer = HeapAlloc(GetProcessHeap(), 0, maxLen*sizeof(WCHAR));
    if (!pathBuffer) return NULL;
    *pathBuffer = 0;
    if (!get_item_path(hwndTV, hItem, phRootKey, &pathBuffer, &pathLen, &maxLen)) return NULL;
    return pathBuffer;
}

static LPWSTR get_path_component(LPCWSTR *lplpKeyName) {
     LPCWSTR lpPos = *lplpKeyName;
     LPWSTR lpResult = NULL;
     int len;
     if (!lpPos)
         return NULL;
     while(*lpPos && *lpPos != '\\')
         lpPos++;
     if (*lpPos && lpPos == *lplpKeyName)
         return NULL;
     len = lpPos+1-(*lplpKeyName);
     lpResult = HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
     if (!lpResult) /* that would be very odd */
         return NULL;
     lstrcpynW(lpResult, *lplpKeyName, len);
     *lplpKeyName = *lpPos ? lpPos+1 : NULL;
     return lpResult;
}

HTREEITEM FindPathInTree(HWND hwndTV, LPCWSTR lpKeyName) {
    TVITEMEXW tvi;
    WCHAR buf[261]; /* tree view has 260 character limitation on item name */
    HTREEITEM hRoot, hItem, hOldItem;
    BOOL valid_path;

    buf[260] = '\0';
    hRoot = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    hItem = hRoot;
    SendMessageW(hwndTV, TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem );
    hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
    hOldItem = hItem;
    valid_path = FALSE;
    while(1) {
        LPWSTR lpItemName = get_path_component(&lpKeyName);

        if (lpItemName) {
            while(hItem) {
                tvi.mask = TVIF_TEXT | TVIF_HANDLE;
                tvi.hItem = hItem;
                tvi.pszText = buf;
                tvi.cchTextMax = 260;
                SendMessageW(hwndTV, TVM_GETITEMW, 0, (LPARAM) &tvi);
                if (!lstrcmpiW(tvi.pszText, lpItemName)) {
                     valid_path = TRUE;
                     SendMessageW(hwndTV, TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem );
                     if (!lpKeyName)
                     {
                         HeapFree(GetProcessHeap(), 0, lpItemName);
                         return hItem;
                     }
                     hOldItem = hItem;
                     hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
                     break;
                }
                hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
            }
            HeapFree(GetProcessHeap(), 0, lpItemName);
            if (!hItem)
                return valid_path ? hOldItem : hRoot;
        }
        else
            return valid_path ? hItem : hRoot;
    }
}

BOOL DeleteNode(HWND hwndTV, HTREEITEM hItem)
{
    if (!hItem) hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CARET, 0);
    if (!hItem) return FALSE;
    return (BOOL)SendMessageW(hwndTV, TVM_DELETEITEM, 0, (LPARAM)hItem);
}

/* Add an entry to the tree. Only give hKey for root nodes (HKEY_ constants) */
static HTREEITEM AddEntryToTree(HWND hwndTV, HTREEITEM hParent, LPWSTR label, HKEY hKey, DWORD dwChildren)
{
    TVINSERTSTRUCTW tvins;

    if (hKey) {
        if (RegQueryInfoKeyW(hKey, 0, 0, 0, &dwChildren, 0, 0, 0, 0, 0, 0, 0) != ERROR_SUCCESS) {
            dwChildren = 0;
        }
    }

    tvins.u.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    tvins.u.item.pszText = label;
    tvins.u.item.cchTextMax = lstrlenW(label);
    tvins.u.item.iImage = Image_Closed;
    tvins.u.item.iSelectedImage = Image_Open;
    tvins.u.item.cChildren = dwChildren;
    tvins.u.item.lParam = (LPARAM)hKey;
    tvins.hInsertAfter = hKey ? TVI_LAST : TVI_SORT;
    tvins.hParent = hParent;

    return TreeView_InsertItemW(hwndTV, &tvins);
}

static BOOL match_string(LPCWSTR sstring1, LPCWSTR sstring2, int mode)
{
    if (mode & SEARCH_WHOLE)
        return !lstrcmpiW(sstring1, sstring2);
    else
        return NULL != StrStrIW(sstring1, sstring2);
}

static BOOL match_item(HWND hwndTV, HTREEITEM hItem, LPCWSTR sstring, int mode, int *row)
{
    TVITEMW item;
    WCHAR keyname[KEY_MAX_LEN];
    item.mask = TVIF_TEXT;
    item.hItem = hItem;
    item.pszText = keyname;
    item.cchTextMax = KEY_MAX_LEN;
    if (!TreeView_GetItemW(hwndTV, &item)) return FALSE;
    if ((mode & SEARCH_KEYS) && match_string(keyname, sstring, mode)) {
        *row = -1;
        return TRUE;
    }

    if (mode & (SEARCH_VALUES | SEARCH_CONTENT)) {
        int i, adjust;
        WCHAR *valName, *KeyPath;
        HKEY hKey, hRoot;
        DWORD lenName, lenNameMax, lenValueMax;
        WCHAR *buffer = NULL;
        
        KeyPath = GetItemPath(hwndTV, hItem, &hRoot);

        if (!KeyPath || !hRoot)
             return FALSE;

        if (RegOpenKeyExW(hRoot, KeyPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            HeapFree(GetProcessHeap(), 0, KeyPath);
            return FALSE;
        }

        HeapFree(GetProcessHeap(), 0, KeyPath);

        if (ERROR_SUCCESS != RegQueryInfoKeyW(hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &lenNameMax, &lenValueMax, NULL, NULL))
            return FALSE;

        lenName = ++lenNameMax;
        if (!(valName = HeapAlloc(GetProcessHeap(), 0, lenName * sizeof(valName[0]) )))
            return FALSE;

        adjust = 0;
        /* RegEnumValue won't return empty default value, so fake it when dealing with *row,
           which corresponds to list view rows, not value ids */
        if (ERROR_SUCCESS == RegEnumValueW(hKey, 0, valName, &lenName, NULL, NULL, NULL, NULL) && *valName)
            adjust = 1;
        
        i = (*row)-adjust;
        if (i < 0) i = 0;
        while(1) {
            DWORD lenValue = 0, type = 0;
            lenName = lenNameMax;
            
            if (ERROR_SUCCESS != RegEnumValueW(hKey,
                i, valName, &lenName, NULL, &type, NULL, NULL))
                break;
            
            if (mode & SEARCH_VALUES) {
                if (match_string(valName, sstring, mode)) {
                    HeapFree(GetProcessHeap(), 0, valName);
                    HeapFree(GetProcessHeap(), 0, buffer);
                    RegCloseKey(hKey);
                    *row = i+adjust;
                    return TRUE;
                }
            }
            
            if ((mode & SEARCH_CONTENT) && (type == REG_EXPAND_SZ || type == REG_SZ)) {
                if (!buffer)
                    buffer = HeapAlloc(GetProcessHeap(), 0, lenValueMax);
                if (!buffer)
                    break;
                lenName = lenNameMax;
                lenValue = lenValueMax;
                if (ERROR_SUCCESS != RegEnumValueW(hKey, i, valName, &lenName, NULL, &type, (LPBYTE)buffer, &lenValue))
                    break;
                if (match_string(buffer, sstring, mode)) {
                    HeapFree(GetProcessHeap(), 0, valName);
                    HeapFree(GetProcessHeap(), 0, buffer);
                    RegCloseKey(hKey);
                    *row = i+adjust;
                    return TRUE;
                }
            }
                            
            i++;
        }
        HeapFree(GetProcessHeap(), 0, valName);
        HeapFree(GetProcessHeap(), 0, buffer);
        RegCloseKey(hKey);
    }        
    return FALSE;
}

HTREEITEM FindNext(HWND hwndTV, HTREEITEM hItem, LPCWSTR sstring, int mode, int *row)
{
    HTREEITEM hTry, hLast;
    
    hLast = hItem;
    (*row)++;
    if (match_item(hwndTV, hLast, sstring, mode & ~SEARCH_KEYS, row)) {
        return hLast;
    }
    *row = 0;
    
    while(hLast) {
        /* first look in subtree */
        /* no children? maybe we haven't loaded them yet? */
        if (!SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hLast)) {
            UINT state = (UINT)SendMessageW(hwndTV, TVM_GETITEMSTATE, (WPARAM)hLast, -1);
            UpdateExpandingTree(hwndTV, hLast, state);
        }
        hTry = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hLast);
        if (hTry) {
            if (match_item(hwndTV, hTry, sstring, mode, row))
                return hTry;
            hLast = hTry;
            continue;
        }
        /* no more children, maybe there are any siblings? */
        hTry = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hLast);
        if (hTry) {
            if (match_item(hwndTV, hTry, sstring, mode, row))
                return hTry;
            hLast = hTry;
            continue;
        }
        /* no more siblings, look at the next siblings in parent(s) */
        hLast = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hLast);
        if (!hLast)
            return NULL;
        while (hLast && (hTry = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hLast)) == NULL) {
            hLast = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hLast);
        }
        if (match_item(hwndTV, hTry, sstring, mode, row))
            return hTry;
        hLast = hTry;
    }
    return NULL;
}

static BOOL RefreshTreeItem(HWND hwndTV, HTREEITEM hItem)
{
    HKEY hRoot, hKey, hSubKey;
    HTREEITEM childItem;
    LPWSTR KeyPath;
    DWORD dwCount, dwIndex, dwMaxSubKeyLen;
    LPWSTR Name;
    TVITEMW tvItem;
    
    hRoot = NULL;
    KeyPath = GetItemPath(hwndTV, hItem, &hRoot);

    if (!KeyPath || !hRoot)
        return FALSE;

    if (*KeyPath) {
        if (RegOpenKeyExW(hRoot, KeyPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            WINE_TRACE("RegOpenKeyEx failed, %s was probably removed.\n", wine_dbgstr_w(KeyPath));
            return FALSE;
        }
    } else {
        hKey = hRoot;
    }
    HeapFree(GetProcessHeap(), 0, KeyPath);

    if (RegQueryInfoKeyW(hKey, 0, 0, 0, &dwCount, &dwMaxSubKeyLen, 0, 0, 0, 0, 0, 0) != ERROR_SUCCESS) {
        return FALSE;
    }

    /* Set the number of children again */
    tvItem.mask = TVIF_CHILDREN;
    tvItem.hItem = hItem;
    tvItem.cChildren = dwCount;
    if (!TreeView_SetItemW(hwndTV, &tvItem)) {
        return FALSE;
    }

    /* We don't have to bother with the rest if it's not expanded. */
    if (SendMessageW(hwndTV, TVM_GETITEMSTATE, (WPARAM)hItem, TVIS_EXPANDED) == 0) {
        RegCloseKey(hKey);
        return TRUE;
    }

    dwMaxSubKeyLen++; /* account for the \0 terminator */
    if (!(Name = HeapAlloc(GetProcessHeap(), 0, dwMaxSubKeyLen * sizeof(WCHAR)))) {
        return FALSE;
    }
    tvItem.cchTextMax = dwMaxSubKeyLen;
    if (!(tvItem.pszText = HeapAlloc(GetProcessHeap(), 0, dwMaxSubKeyLen * sizeof(WCHAR)))) {
        HeapFree(GetProcessHeap(), 0, Name);
        return FALSE;
    }

    /* Now go through all the children in the registry, and check if any have to be added. */
    for (dwIndex = 0; dwIndex < dwCount; dwIndex++) {
        DWORD cName = dwMaxSubKeyLen, dwSubCount;
        BOOL found;

        found = FALSE;
        if (RegEnumKeyExW(hKey, dwIndex, Name, &cName, 0, 0, 0, NULL) != ERROR_SUCCESS) {
            continue;
        }

        /* Find the number of children of the node. */
        dwSubCount = 0;
        if (RegOpenKeyExW(hKey, Name, 0, KEY_QUERY_VALUE, &hSubKey) == ERROR_SUCCESS) {
            if (RegQueryInfoKeyW(hSubKey, 0, 0, 0, &dwSubCount, 0, 0, 0, 0, 0, 0, 0) != ERROR_SUCCESS) {
                dwSubCount = 0;
            }
            RegCloseKey(hSubKey);
        }

        /* Check if the node is already in there. */
        for (childItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem); childItem;
                childItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)childItem)) {
            tvItem.mask = TVIF_TEXT;
            tvItem.hItem = childItem;
            if (!TreeView_GetItemW(hwndTV, &tvItem)) {
                HeapFree(GetProcessHeap(), 0, Name);
                HeapFree(GetProcessHeap(), 0, tvItem.pszText);
                return FALSE;
            }

            if (!lstrcmpiW(tvItem.pszText, Name)) {
                found = TRUE;
                break;
            }
        }

        if (found == FALSE) {
            WINE_TRACE("New subkey %s\n", wine_dbgstr_w(Name));
            AddEntryToTree(hwndTV, hItem, Name, NULL, dwSubCount);
        }
    }
    HeapFree(GetProcessHeap(), 0, Name);
    HeapFree(GetProcessHeap(), 0, tvItem.pszText);
    RegCloseKey(hKey);

    /* Now go through all the children in the tree, and check if any have to be removed. */
    childItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
    while (childItem) {
        HTREEITEM nextItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)childItem);
        if (RefreshTreeItem(hwndTV, childItem) == FALSE) {
            SendMessageW(hwndTV, TVM_DELETEITEM, 0, (LPARAM)childItem);
        }
        childItem = nextItem;
    }

    return TRUE;
}

BOOL RefreshTreeView(HWND hwndTV)
{
    HTREEITEM hItem;
    HTREEITEM hSelectedItem;
    HCURSOR hcursorOld;
    HTREEITEM hRoot;

    WINE_TRACE("\n");
    hSelectedItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CARET, 0);
    hcursorOld = SetCursor(LoadCursorW(NULL, (LPCWSTR)IDC_WAIT));
    SendMessageW(hwndTV, WM_SETREDRAW, FALSE, 0);

    hRoot = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hRoot);
    while (hItem) {
        RefreshTreeItem(hwndTV, hItem);
        hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
    }

    SendMessageW(hwndTV, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hwndTV, NULL, FALSE);
    SetCursor(hcursorOld);
    
    /* We reselect the currently selected node, this will prompt a refresh of the listview. */
    SendMessageW(hwndTV, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hSelectedItem);
    return TRUE;
}

HTREEITEM InsertNode(HWND hwndTV, HTREEITEM hItem, LPWSTR name)
{
    WCHAR buf[MAX_NEW_KEY_LEN];
    HTREEITEM hNewItem = 0;
    TVITEMEXW item;

    if (!hItem) hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CARET, 0);
    if (!hItem) return FALSE;
    if (SendMessageW(hwndTV, TVM_GETITEMSTATE, (WPARAM)hItem, TVIS_EXPANDEDONCE) & TVIS_EXPANDEDONCE) {
        hNewItem = AddEntryToTree(hwndTV, hItem, name, 0, 0);
    } else {
	item.mask = TVIF_CHILDREN | TVIF_HANDLE;
	item.hItem = hItem;
	if (!TreeView_GetItemW(hwndTV, &item)) return FALSE;
	item.cChildren = 1;
	if (!TreeView_SetItemW(hwndTV, &item)) return FALSE;
    }
    SendMessageW(hwndTV, TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem );
    if (!hNewItem) {
        for(hNewItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem); hNewItem;
            hNewItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hNewItem)) {
            item.mask = TVIF_HANDLE | TVIF_TEXT;
            item.hItem = hNewItem;
            item.pszText = buf;
            item.cchTextMax = COUNT_OF(buf);
            if (!TreeView_GetItemW(hwndTV, &item)) continue;
            if (lstrcmpW(name, item.pszText) == 0) break;
        }
    }
    if (hNewItem)
        SendMessageW(hwndTV, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hNewItem);

    return hNewItem;
}

HWND StartKeyRename(HWND hwndTV)
{
    HTREEITEM hItem;

    if(!(hItem = (HTREEITEM)SendMessageW(hwndTV, TVM_GETNEXTITEM, TVGN_CARET, 0))) return 0;
    SetWindowLongPtrW(hwndTV, GWLP_USERDATA, 1);
    return (HWND)SendMessageW(hwndTV, TVM_EDITLABELW, 0, (LPARAM)hItem);
}

static BOOL InitTreeViewItems(HWND hwndTV, LPWSTR pHostName)
{
    TVINSERTSTRUCTW tvins;
    HTREEITEM hRoot;
    static WCHAR hkcr[] = {'H','K','E','Y','_','C','L','A','S','S','E','S','_','R','O','O','T',0},
                 hkcu[] = {'H','K','E','Y','_','C','U','R','R','E','N','T','_','U','S','E','R',0},
                 hklm[] = {'H','K','E','Y','_','L','O','C','A','L','_','M','A','C','H','I','N','E',0},
                 hku[]  = {'H','K','E','Y','_','U','S','E','R','S',0},
                 hkcc[] = {'H','K','E','Y','_','C','U','R','R','E','N','T','_','C','O','N','F','I','G',0},
                 hkdd[] = {'H','K','E','Y','_','D','Y','N','_','D','A','T','A',0};

    tvins.u.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    /* Set the text of the item.  */
    tvins.u.item.pszText = pHostName;
    tvins.u.item.cchTextMax = lstrlenW(pHostName);
    /* Assume the item is not a parent item, so give it an image.  */
    tvins.u.item.iImage = Image_Root;
    tvins.u.item.iSelectedImage = Image_Root;
    tvins.u.item.cChildren = 5;
    /* Save the heading level in the item's application-defined data area.  */
    tvins.u.item.lParam = 0;
    tvins.hInsertAfter = TVI_FIRST;
    tvins.hParent = TVI_ROOT;
    /* Add the item to the tree view control.  */
    if (!(hRoot = TreeView_InsertItemW(hwndTV, &tvins))) return FALSE;

    if (!AddEntryToTree(hwndTV, hRoot, hkcr, HKEY_CLASSES_ROOT, 1)) return FALSE;
    if (!AddEntryToTree(hwndTV, hRoot, hkcu, HKEY_CURRENT_USER, 1)) return FALSE;
    if (!AddEntryToTree(hwndTV, hRoot, hklm, HKEY_LOCAL_MACHINE, 1)) return FALSE;
    if (!AddEntryToTree(hwndTV, hRoot, hku, HKEY_USERS, 1)) return FALSE;
    if (!AddEntryToTree(hwndTV, hRoot, hkcc, HKEY_CURRENT_CONFIG, 1)) return FALSE;
    if (!AddEntryToTree(hwndTV, hRoot, hkdd, HKEY_DYN_DATA, 1)) return FALSE;

    /* expand and select host name */
    SendMessageW(hwndTV, TVM_EXPAND, TVE_EXPAND, (LPARAM)hRoot );
    SendMessageW(hwndTV, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hRoot);
    return TRUE;
}


/*
 * InitTreeViewImageLists - creates an image list, adds three bitmaps
 * to it, and associates the image list with a tree view control.
 * Returns TRUE if successful, or FALSE otherwise.
 * hwndTV - handle to the tree view control.
 */
static BOOL InitTreeViewImageLists(HWND hwndTV)
{
    HIMAGELIST himl;  /* handle to image list  */
    HICON hico;       /* handle to icon  */

    /* Create the image list.  */
    if ((himl = ImageList_Create(CX_ICON, CY_ICON,
                                 ILC_MASK, 0, NUM_ICONS)) == NULL)
        return FALSE;

    /* Add the open file, closed file, and document bitmaps.  */
    hico = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_OPEN_FILE));
    Image_Open = ImageList_AddIcon(himl, hico);

    hico = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_CLOSED_FILE));
    Image_Closed = ImageList_AddIcon(himl, hico);

    hico = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_ROOT));
    Image_Root = ImageList_AddIcon(himl, hico);

    /* Fail if not all of the images were added.  */
    if (ImageList_GetImageCount(himl) < NUM_ICONS)
    {
      return FALSE;
    }

    /* Associate the image list with the tree view control.  */
    SendMessageW(hwndTV, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)himl);

    return TRUE;
}

BOOL UpdateExpandingTree(HWND hwndTV, HTREEITEM hItem, int state)
{
    DWORD dwCount, dwIndex, dwMaxSubKeyLen;
    HKEY hRoot, hNewKey, hKey;
    LPWSTR keyPath;
    LPWSTR Name;
    LONG errCode;
    HCURSOR hcursorOld;
    TVITEMW item;

    static int expanding;
    if (expanding) return FALSE;
    if (state & TVIS_EXPANDEDONCE ) {
        return TRUE;
    }
    expanding = TRUE;
    hcursorOld = SetCursor(LoadCursorW(NULL, (LPCWSTR)IDC_WAIT));
    SendMessageW(hwndTV, WM_SETREDRAW, FALSE, 0);

    keyPath = GetItemPath(hwndTV, hItem, &hRoot);
    if (!keyPath) goto done;

    if (*keyPath) {
        errCode = RegOpenKeyExW(hRoot, keyPath, 0, KEY_READ, &hNewKey);
        if (errCode != ERROR_SUCCESS) goto done;
    } else {
	hNewKey = hRoot;
    }

    errCode = RegQueryInfoKeyW(hNewKey, 0, 0, 0, &dwCount, &dwMaxSubKeyLen, 0, 0, 0, 0, 0, 0);
    if (errCode != ERROR_SUCCESS) goto done;
    dwMaxSubKeyLen++; /* account for the \0 terminator */
    Name = HeapAlloc(GetProcessHeap(), 0, dwMaxSubKeyLen * sizeof(WCHAR));
    if (!Name) goto done;

    for (dwIndex = 0; dwIndex < dwCount; dwIndex++) {
        DWORD cName = dwMaxSubKeyLen, dwSubCount;

        errCode = RegEnumKeyExW(hNewKey, dwIndex, Name, &cName, 0, 0, 0, 0);
        if (errCode != ERROR_SUCCESS) continue;
        errCode = RegOpenKeyExW(hNewKey, Name, 0, KEY_QUERY_VALUE, &hKey);
        if (errCode == ERROR_SUCCESS) {
            errCode = RegQueryInfoKeyW(hKey, 0, 0, 0, &dwSubCount, 0, 0, 0, 0, 0, 0, 0);
            RegCloseKey(hKey);
        }
        if (errCode != ERROR_SUCCESS) dwSubCount = 0;
        AddEntryToTree(hwndTV, hItem, Name, NULL, dwSubCount);
    }
    RegCloseKey(hNewKey);
    HeapFree(GetProcessHeap(), 0, Name);

done:
    item.mask = TVIF_STATE;
    item.hItem = hItem;
    item.stateMask = TVIS_EXPANDEDONCE;
    item.state = TVIS_EXPANDEDONCE;
    SendMessageW(hwndTV, TVM_SETITEMW, 0, (LPARAM)&item);
    SendMessageW(hwndTV, WM_SETREDRAW, TRUE, 0);
    SetCursor(hcursorOld);
    expanding = FALSE;
    HeapFree(GetProcessHeap(), 0, keyPath);

    return TRUE;
}

BOOL OnTreeExpanding(HWND hwndTV, NMTREEVIEWW* pnmtv)
{
    return UpdateExpandingTree(hwndTV, pnmtv->itemNew.hItem, pnmtv->itemNew.state);
}


/*
 * CreateTreeView - creates a tree view control.
 * Returns the handle to the new control if successful, or NULL otherwise.
 * hwndParent - handle to the control's parent window.
 */
HWND CreateTreeView(HWND hwndParent, LPWSTR pHostName, UINT id)
{
    RECT rcClient;
    HWND hwndTV;
    WCHAR TreeView[] = {'T','r','e','e',' ','V','i','e','w',0};

    /* Get the dimensions of the parent window's client area, and create the tree view control.  */
    GetClientRect(hwndParent, &rcClient);
    hwndTV = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, TreeView,
                            WS_VISIBLE | WS_CHILD | WS_TABSTOP | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_EDITLABELS,
                            0, 0, rcClient.right, rcClient.bottom,
                            hwndParent, ULongToHandle(id), hInst, NULL);
    SendMessageW(hwndTV, TVM_SETUNICODEFORMAT, TRUE, 0);
    /* Initialize the image list, and add items to the control.  */
    if (!InitTreeViewImageLists(hwndTV) || !InitTreeViewItems(hwndTV, pHostName)) {
        DestroyWindow(hwndTV);
        return NULL;
    }
    return hwndTV;
}
