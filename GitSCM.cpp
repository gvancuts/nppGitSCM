///----------------------------------------------------------------------------
/// Copyright (c) 2008-2010
/// Brandon Cannaday
/// Paranoid Ferret Productions (support@paranoidferret.com)
///
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either
/// version 2 of the License, or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
///----------------------------------------------------------------------------

#include "PluginDefinition.h"
#include "resource.h"

extern FuncItem     funcItem[nbFunc];
extern HINSTANCE    g_hInst;
extern NppData      nppData;
extern bool         g_NppReady;
extern toolbarIcons g_TBGit;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  reasonForCall, LPVOID /*lpReserved*/ )
{
    switch ( reasonForCall )
    {
        case DLL_PROCESS_ATTACH:
            g_hInst = ( HINSTANCE )hModule;
            pluginInit( hModule );
            break;

        case DLL_PROCESS_DETACH:
            pluginCleanUp();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}


extern "C" __declspec( dllexport ) void setInfo( NppData notpadPlusData )
{
    nppData = notpadPlusData;
    commandMenuInit();
}

extern "C" __declspec( dllexport ) const TCHAR *getName()
{
    return NPP_PLUGIN_NAME;
}

extern "C" __declspec( dllexport ) FuncItem *getFuncsArray( int *nbF )
{
    *nbF = nbFunc;
    return funcItem;
}

extern "C" __declspec( dllexport ) void beNotified( SCNotification *notifyCode )
{
    switch (notifyCode->nmhdr.code)
    {
        case NPPN_TBMODIFICATION:
            g_TBGit.hToolbarBmp = (HBITMAP)::LoadImage((HINSTANCE)g_hInst, MAKEINTRESOURCE(IDB_TB_GIT), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
            ::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[DOCKABLE_INDEX]._cmdID, (LPARAM)&g_TBGit);
            break;

        case NPPN_READY:
            g_NppReady = true;
            updatePanelLoc();
            break;

// TODO:2019-12-25:MVINCENT: if updatePanel() is used, Causes very slow tab switching
//                           Convert Process.cpp CreateProcess to CreateThread
        case NPPN_BUFFERACTIVATED:
        {
            if ( g_NppReady )    
                updatePanelLoc();
        }
        break;

        case NPPN_FILESAVED:
            updatePanel();
            break;

        case NPPN_FILEOPENED:
        {
            if ( g_NppReady )    
                updatePanel();
        }
        break;

        case NPPN_SHUTDOWN:
            commandMenuCleanUp();
            break;

        default:
            return;
    }
}

// Here you can process the Npp Messages
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
// http://sourceforge.net/forum/forum.php?forum_id=482781
//
extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return TRUE;
}

#ifdef UNICODE
extern "C" __declspec( dllexport ) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE
