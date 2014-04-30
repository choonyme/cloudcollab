// generated 2006/12/6 13:53:44 EST by cjx@cjx-desktop.vislab2.usyd.edu.au
// using glademm V2.6.0
//
// newer (non customized) versions of this file go to wndMain.hh_new

// you might replace
//    class foo : public foo_glade { ... };
// by
//    typedef foo_glade foo;
// if you didn't make any modifications to the widget

#ifndef _WNDMAIN_HH
#include "wndMain_glade.hh"
#include <gtkmm.h>
#define _WNDMAIN_HH

using namespace std;

extern void launchVP(int listen_port, char* server, int port);

class wndMain : public wndMain_glade
{  
	protected:
		typedef Gtk::TreeModel::Children type_children;
	
		class ModelColumns : public Gtk::TreeModel::ColumnRecord{
			public:
				ModelColumns()
				{ add(m_col_name); }
				
				Gtk::TreeModelColumn<Glib::ustring> m_col_name;
		};
	
		ModelColumns m_Columns;
		Glib::RefPtr<Gtk::ListStore> m_refTreeModelAllServices;
		Glib::RefPtr<Gtk::ListStore> m_refTreeModelSelectedServices;
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;
		
	public:
		wndMain();
		void HandleServiceDiscoveredImpl(const char *name);
		void HandleServiceDisconnectedImpl(const char *name);
	
	private:
        void on_m_txtListenPort_changed();
        void on_m_txtDesktopIP_changed();
        void on_m_txtDesktopPort_changed();
        void on_btnAddServices_clicked();
        void on_btnRemoveServices_clicked();
        void on_btnLaunch_clicked();
        void on_m_btnAddAllServices_clicked();
		void launchVP(int listen_port, const char* server, int port);
		void selected_lstAllServices(const Gtk::TreeModel::iterator& iter);
		void selected_lstSelectedServices(const Gtk::TreeModel::iterator& iter);
		Glib::ustring checkNumeric(Glib::ustring str);
		Glib::ustring checkIP(Glib::ustring str);
};
#endif
