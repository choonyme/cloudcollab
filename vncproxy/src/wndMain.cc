// generated 2006/12/6 13:53:44 EST by cjx@cjx-desktop.vislab2.usyd.edu.au
// using glademm V2.6.0
//
// newer (non customized) versions of this file go to wndMain.cc_new

// This file is for your program, I won't touch it again!

#include "config.h"
#include "wndMain.hh"
#include "vncproxy.h"
#include "bonjour_browser.h"
#include "dialogPassword.hh"

void* p_wndMainObj;

void HandleServiceDiscovered(const char *name){
	((wndMain*) p_wndMainObj)->HandleServiceDiscoveredImpl(name);
}

void HandleServiceDisconnected(const char *name){
	((wndMain*) p_wndMainObj)->HandleServiceDisconnectedImpl(name);
}

char *inputPassword(rfbClient* client){
	char* p= (char*) malloc(9);
	dialogPassword dlgPassword;
		
	dlgPassword.run();
	
	Glib::ustring password = dlgPassword.getPassword();
	strcpy(p, password.c_str());
	
    return p;
}

wndMain::wndMain(){
	p_wndMainObj = this;
	
	m_btnAddServices->set_sensitive(false);
	m_btnRemoveServices->set_sensitive(false);
	m_btnAddAllServices->set_sensitive(false);
	
	//Create Tree model
	m_refTreeModelAllServices = Gtk::ListStore::create(m_Columns);
	m_refTreeModelSelectedServices = Gtk::ListStore::create(m_Columns);
	m_lstAllServices->set_model(m_refTreeModelAllServices);
	m_lstAllServices->append_column("Name", m_Columns.m_col_name);
	m_lstSelectedServices->set_model(m_refTreeModelSelectedServices);
	m_lstSelectedServices->append_column("Name", m_Columns.m_col_name);
	
	refTreeSelection = m_lstAllServices->get_selection();
	refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	refTreeSelection = m_lstSelectedServices->get_selection();
	refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	
	initServiceDiscovery();
	process(0, 999999);
	
	m_txtListenPort->set_text("3");
	m_txtDesktopIP->set_text("127.0.0.1");
	m_txtDesktopPort->set_text("1");
}

void wndMain::HandleServiceDiscoveredImpl(const char *name){
	Gtk::TreeModel::Row row = *(m_refTreeModelAllServices->append());
	row[m_Columns.m_col_name] = name;
	
	printf("Service connection callled...\n");
}

void wndMain::HandleServiceDisconnectedImpl(const char *name){
	//Delete service name in lstAllServices if it exists.
	type_children children = m_refTreeModelAllServices->children();
	
	printf("Service Disconnection called...\n");
	
	for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		
		//Delete row
		Glib::ustring str = row[m_Columns.m_col_name];
		if( str.compare(name) == 0 ){
			m_refTreeModelAllServices->erase(iter);
			return;
		}
	}
	
	//Remove service if service is connected.
	children = m_refTreeModelSelectedServices->children();
	printf("Service Disconnection called...\n");
	for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		Gtk::TreeModel::Row row = *iter;
		
		//Delete row
		Glib::ustring str = row[m_Columns.m_col_name];
		if( str.compare(name) == 0 ){	
			//Disconnect service and remove service name from lstSelectedServices
			disconnectService(str.c_str());
			m_refTreeModelSelectedServices->erase(iter);
			
			return;
		}
	}
}

void wndMain::on_m_txtListenPort_changed()
{
	Glib::ustring result = checkNumeric(m_txtListenPort->get_text());
	m_txtListenPort->set_text(result);
}

void wndMain::on_m_txtDesktopPort_changed()
{
	Glib::ustring result = checkNumeric(m_txtDesktopPort->get_text());
	m_txtDesktopPort->set_text(result);
}

Glib::ustring wndMain::checkNumeric(Glib::ustring str){
	Glib::ustring result;

	Glib::ustring::iterator cur;
	for(cur=str.begin(); cur != str.end(); ++cur){
		char chr = *cur;
		if( chr >= '0' && chr <= '9')
			result += *cur;
	}
	
	return result;
}

void wndMain::on_m_txtDesktopIP_changed()
{
	Glib::ustring result = checkIP(m_txtDesktopIP->get_text());
	m_txtDesktopIP->set_text(result);
}

//Partial IP check
Glib::ustring wndMain::checkIP(Glib::ustring str){
	Glib::ustring result;
	int i;
	
	Glib::ustring::iterator cur;

	i=3;
	for(cur=str.begin(); cur != str.end(); ++cur){
		char chr = *cur;
		
		--i;
		
		if(chr == '.')
			i=3;

		if(i < 0)
			break;
		
		if( chr >= '0' && chr <= '9' || chr == '.')
			result += *cur;
	}
	
	return result;
}

//"Add" button clicked
void wndMain::on_btnAddServices_clicked()
{
	std::list<Gtk::TreePath> list = m_lstAllServices->get_selection()->get_selected_rows(); 
	std::list<Gtk::TreeIter> iter_list; 

	// Convert all paths to iterators. Then remove them one by one.
	for(std::list<Gtk::TreePath>::const_iterator iter = list.begin(); iter != list.end(); ++ iter) 
	{ 
		iter_list.push_back(m_refTreeModelAllServices->get_iter(*iter) );
	} 
	 
	for(std::list<Gtk::TreeIter>::const_iterator iter = iter_list.begin(); iter != iter_list.end(); ++ iter) 
	{
		selected_lstAllServices(*iter);
	}
}

//"Add All" button clicked
void wndMain::on_m_btnAddAllServices_clicked()
{
	type_children children = m_refTreeModelAllServices->children(); 
	std::list<Gtk::TreeRow> iter_list; 

	// Convert all paths to iterators. Then remove them one by one.
	for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		iter_list.push_back(*iter);
	}
	 
	for(std::list<Gtk::TreeRow>::const_iterator iter = iter_list.begin(); iter != iter_list.end(); ++ iter) 
	{
		selected_lstAllServices(*iter);
	}
}

void wndMain::selected_lstAllServices(const Gtk::TreeModel::iterator& iter){
	Gtk::TreeModel::Row row = *iter;
	Glib::ustring str = row[m_Columns.m_col_name];
	
	resolveService(str.c_str(), 0);
	
	row = *(m_refTreeModelSelectedServices->append());
	row[m_Columns.m_col_name] = str;
	
	m_refTreeModelAllServices->erase(*iter);
}

//"Remove" button clicked
void wndMain::on_btnRemoveServices_clicked()
{
	std::list<Gtk::TreePath> list = m_lstSelectedServices->get_selection()->get_selected_rows(); 
	std::list<Gtk::TreeIter> iter_list; 

	// Convert all paths to iterators. Then remove them one by one.
	for(std::list<Gtk::TreePath>::const_iterator iter = list.begin(); iter != list.end(); ++ iter) 
	{ 
		iter_list.push_back(m_refTreeModelSelectedServices->get_iter(*iter) );
	} 
	 
	for(std::list<Gtk::TreeIter>::const_iterator iter = iter_list.begin(); iter != iter_list.end(); ++ iter) 
	{
		selected_lstSelectedServices(*iter);
	}
}

void wndMain::selected_lstSelectedServices(const Gtk::TreeModel::iterator& iter){
	Gtk::TreeModel::Row row = *iter;
	Glib::ustring str = row[m_Columns.m_col_name];
	
	//Disconnect service and remove service name from lstSelectedServices
	disconnectService(str.c_str());
	m_refTreeModelSelectedServices->erase(*iter);
	
	//Add service name back to lstAllServices
	row = *(m_refTreeModelAllServices->append());
	row[m_Columns.m_col_name] = str;
}

void wndMain::on_btnLaunch_clicked()
{
	launchVP(5900 + atoi(m_txtListenPort->get_text().c_str()), 
		m_txtDesktopIP->get_text().c_str(), 
		5900 + atoi(m_txtDesktopPort->get_text().c_str()));
}

void wndMain::launchVP(int listen_port, const char* server, int port){
	static resource_t resource;
	
	srand(time(0));
	printf("Connecting to %s:%d listening on %d.\n", server, port, listen_port);
   	resource = initvnc(server, port, listen_port);

   	if (resource < 0)
   	{
		Gtk::MessageDialog dialog(*this, "Could not initialise VNC.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, false);
		printf("Could not initialise VNC\n");
  		dialog.run();
   	}else{
		m_btnLaunch->set_sensitive(false);
		m_btnAddServices->set_sensitive(true);
		m_btnRemoveServices->set_sensitive(true);
		m_btnAddAllServices->set_sensitive(true);
	}
}
