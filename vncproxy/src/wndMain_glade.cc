// generated 2007/1/16 14:41:20 EST by cjx@cjx-desktop.vislab2.usyd.edu.au
// using glademm V2.6.0
//
// DO NOT EDIT THIS FILE ! It was created using
// glade-- /home/cjx/proj/sorc/trunk/src/vncproxy/VNCProxy.glade
// for gtk 2.10.6 and gtkmm 2.10.2
//
// Please modify the corresponding derived classes in ./src/wndMain.cc


#if defined __GNUC__ && __GNUC__ < 3
#error This program will crash if compiled with g++ 2.x
// see the dynamic_cast bug in the gtkmm FAQ
#endif //
#include "config.h"
/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif
#include <gtkmmconfig.h>
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#include <sigc++/compatibility.h>
#define GMM_GTKMM_22_24(a,b) b
#else //gtkmm 2.2
#define GMM_GTKMM_22_24(a,b) a
#endif //
#include "wndMain_glade.hh"
#include <gdk/gdkkeysyms.h>
#include <gtkmm/accelgroup.h>

wndMain_glade::wndMain_glade(
) : Gtk::Window(Gtk::WINDOW_TOPLEVEL)
{  wndMain = this;
   gmm_data = new GlademmData(get_accel_group());
   m_lblTitle = Gtk::manage(new class Gtk::Label(_("VNCProxy:\n"
		"Please add the services that is to be used and click the \"Launch\" button.")));
   m_label4 = Gtk::manage(new class Gtk::Label(_("Listen port:")));
   m_txtListenPort = Gtk::manage(new class Gtk::Entry());
   m_label8 = Gtk::manage(new class Gtk::Label(_("IP Address:")));
   m_txtDesktopIP = Gtk::manage(new class Gtk::Entry());
   m_label9 = Gtk::manage(new class Gtk::Label(_("Port:")));
   m_txtDesktopPort = Gtk::manage(new class Gtk::Entry());
   m_hbox3 = Gtk::manage(new class Gtk::HBox(false, 0));
   m_alignment1 = Gtk::manage(new class Gtk::Alignment(0.5, 0.5, 1, 1));
   m_label7 = Gtk::manage(new class Gtk::Label(_("<b>VNC Desktop:</b>")));
   m_frame1 = Gtk::manage(new class Gtk::Frame());
   m_hbox2 = Gtk::manage(new class Gtk::HBox(false, 0));
   m_lstAllServices = Gtk::manage(new class Gtk::TreeView());
   m_scrolledwindow1 = Gtk::manage(new class Gtk::ScrolledWindow());
   m_alignment2 = Gtk::manage(new class Gtk::Alignment(0.5, 0.5, 1, 1));
   m_label2 = Gtk::manage(new class Gtk::Label(_("<b>Available services:</b>")));
   m_frame2 = Gtk::manage(new class Gtk::Frame());
   m_btnAddServices = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-add")));
   image1 = Gtk::manage(new class Gtk::Image(Gtk::StockID("gtk-goto-last"), Gtk::IconSize(4)));
   label1 = Gtk::manage(new class Gtk::Label(_("Add All")));
   hbox1 = Gtk::manage(new class Gtk::HBox(false, 2));
   alignment1 = Gtk::manage(new class Gtk::Alignment(0.5, 0.5, 0, 0));
   m_btnAddAllServices = Gtk::manage(new class Gtk::Button());
   m_btnRemoveServices = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-remove")));
   m_vbox2 = Gtk::manage(new class Gtk::VBox(true, 0));
   m_lstSelectedServices = Gtk::manage(new class Gtk::TreeView());
   m_scrolledwindow2 = Gtk::manage(new class Gtk::ScrolledWindow());
   m_alignment3 = Gtk::manage(new class Gtk::Alignment(0.5, 0.5, 1, 1));
   m_label3 = Gtk::manage(new class Gtk::Label(_("<b>Selected services:</b>")));
   m_frame3 = Gtk::manage(new class Gtk::Frame());
   m_hbox1 = Gtk::manage(new class Gtk::HBox(false, 0));
   m_btnLaunch = Gtk::manage(new class Gtk::Button(_("Launch...")));
   m_vbox1 = Gtk::manage(new class Gtk::VBox(false, 0));
   m_lblTitle->set_alignment(0.5,0.5);
   m_lblTitle->set_padding(0,0);
   m_lblTitle->set_justify(Gtk::JUSTIFY_LEFT);
   m_lblTitle->set_line_wrap(false);
   m_lblTitle->set_use_markup(false);
   m_lblTitle->set_selectable(false);
   m_label4->set_alignment(0.5,0.5);
   m_label4->set_padding(0,0);
   m_label4->set_justify(Gtk::JUSTIFY_LEFT);
   m_label4->set_line_wrap(false);
   m_label4->set_use_markup(false);
   m_label4->set_selectable(false);
   m_txtListenPort->set_flags(Gtk::CAN_FOCUS);
   m_txtListenPort->set_visibility(true);
   m_txtListenPort->set_editable(true);
   m_txtListenPort->set_max_length(0);
   m_txtListenPort->set_text(_(""));
   m_txtListenPort->set_has_frame(true);
   m_txtListenPort->set_activates_default(false);
   m_label8->set_alignment(0.5,0.5);
   m_label8->set_padding(0,0);
   m_label8->set_justify(Gtk::JUSTIFY_LEFT);
   m_label8->set_line_wrap(false);
   m_label8->set_use_markup(false);
   m_label8->set_selectable(false);
   m_txtDesktopIP->set_flags(Gtk::CAN_FOCUS);
   m_txtDesktopIP->set_visibility(true);
   m_txtDesktopIP->set_editable(true);
   m_txtDesktopIP->set_max_length(15);
   m_txtDesktopIP->set_text(_(""));
   m_txtDesktopIP->set_has_frame(true);
   m_txtDesktopIP->set_activates_default(false);
   m_label9->set_alignment(0.5,0.5);
   m_label9->set_padding(0,0);
   m_label9->set_justify(Gtk::JUSTIFY_LEFT);
   m_label9->set_line_wrap(false);
   m_label9->set_use_markup(false);
   m_label9->set_selectable(false);
   m_txtDesktopPort->set_flags(Gtk::CAN_FOCUS);
   m_txtDesktopPort->set_visibility(true);
   m_txtDesktopPort->set_editable(true);
   m_txtDesktopPort->set_max_length(0);
   m_txtDesktopPort->set_text(_(""));
   m_txtDesktopPort->set_has_frame(true);
   m_txtDesktopPort->set_activates_default(false);
   m_hbox3->pack_start(*m_label8, Gtk::PACK_SHRINK, 0);
   m_hbox3->pack_start(*m_txtDesktopIP);
   m_hbox3->pack_start(*m_label9, Gtk::PACK_SHRINK, 0);
   m_hbox3->pack_start(*m_txtDesktopPort);
   m_alignment1->add(*m_hbox3);
   m_label7->set_alignment(0.5,0.5);
   m_label7->set_padding(0,0);
   m_label7->set_justify(Gtk::JUSTIFY_LEFT);
   m_label7->set_line_wrap(false);
   m_label7->set_use_markup(true);
   m_label7->set_selectable(false);
   m_frame1->set_shadow_type(Gtk::SHADOW_NONE);
   m_frame1->set_label_align(0,0.5);
   m_frame1->add(*m_alignment1);
   m_frame1->set_label_widget(*m_label7);
   m_hbox2->pack_start(*m_label4, Gtk::PACK_SHRINK, 0);
   m_hbox2->pack_start(*m_txtListenPort);
   m_hbox2->pack_start(*m_frame1);
   m_lstAllServices->set_flags(Gtk::CAN_FOCUS);
   m_lstAllServices->set_headers_visible(true);
   m_lstAllServices->set_rules_hint(false);
   m_lstAllServices->set_reorderable(false);
   m_lstAllServices->set_enable_search(true);
   m_scrolledwindow1->set_flags(Gtk::CAN_FOCUS);
   m_scrolledwindow1->set_shadow_type(Gtk::SHADOW_IN);
   m_scrolledwindow1->set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
   m_scrolledwindow1->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
   m_scrolledwindow1->add(*m_lstAllServices);
   m_alignment2->add(*m_scrolledwindow1);
   m_label2->set_alignment(0.5,0.5);
   m_label2->set_padding(0,0);
   m_label2->set_justify(Gtk::JUSTIFY_LEFT);
   m_label2->set_line_wrap(false);
   m_label2->set_use_markup(true);
   m_label2->set_selectable(false);
   m_frame2->set_shadow_type(Gtk::SHADOW_NONE);
   m_frame2->set_label_align(0,0.5);
   m_frame2->add(*m_alignment2);
   m_frame2->set_label_widget(*m_label2);
   m_btnAddServices->set_flags(Gtk::CAN_FOCUS);
   m_btnAddServices->set_relief(Gtk::RELIEF_NORMAL);
   image1->set_alignment(0.5,0.5);
   image1->set_padding(0,0);
   label1->set_alignment(0.5,0.5);
   label1->set_padding(0,0);
   label1->set_justify(Gtk::JUSTIFY_LEFT);
   label1->set_line_wrap(false);
   label1->set_use_markup(false);
   label1->set_selectable(false);
   hbox1->pack_start(*image1, Gtk::PACK_SHRINK, 0);
   hbox1->pack_start(*label1, Gtk::PACK_SHRINK, 0);
   alignment1->add(*hbox1);
   m_btnAddAllServices->set_flags(Gtk::CAN_FOCUS);
   m_btnAddAllServices->set_relief(Gtk::RELIEF_NORMAL);
   m_btnAddAllServices->add(*alignment1);
   m_btnRemoveServices->set_flags(Gtk::CAN_FOCUS);
   m_btnRemoveServices->set_relief(Gtk::RELIEF_NORMAL);
   m_vbox2->pack_start(*m_btnAddServices, Gtk::PACK_SHRINK, 0);
   m_vbox2->pack_start(*m_btnAddAllServices, Gtk::PACK_SHRINK, 0);
   m_vbox2->pack_start(*m_btnRemoveServices, Gtk::PACK_SHRINK, 0);
   m_lstSelectedServices->set_flags(Gtk::CAN_FOCUS);
   m_lstSelectedServices->set_headers_visible(true);
   m_lstSelectedServices->set_rules_hint(false);
   m_lstSelectedServices->set_reorderable(false);
   m_lstSelectedServices->set_enable_search(true);
   m_scrolledwindow2->set_flags(Gtk::CAN_FOCUS);
   m_scrolledwindow2->set_shadow_type(Gtk::SHADOW_IN);
   m_scrolledwindow2->set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
   m_scrolledwindow2->property_window_placement().set_value(Gtk::CORNER_TOP_LEFT);
   m_scrolledwindow2->add(*m_lstSelectedServices);
   m_alignment3->add(*m_scrolledwindow2);
   m_label3->set_alignment(0.5,0.5);
   m_label3->set_padding(0,0);
   m_label3->set_justify(Gtk::JUSTIFY_LEFT);
   m_label3->set_line_wrap(false);
   m_label3->set_use_markup(true);
   m_label3->set_selectable(false);
   m_frame3->set_shadow_type(Gtk::SHADOW_NONE);
   m_frame3->set_label_align(0,0.5);
   m_frame3->add(*m_alignment3);
   m_frame3->set_label_widget(*m_label3);
   m_hbox1->pack_start(*m_frame2);
   m_hbox1->pack_start(*m_vbox2);
   m_hbox1->pack_start(*m_frame3);
   m_btnLaunch->set_flags(Gtk::CAN_FOCUS);
   m_btnLaunch->set_relief(Gtk::RELIEF_NORMAL);
   m_vbox1->pack_start(*m_lblTitle, Gtk::PACK_SHRINK, 0);
   m_vbox1->pack_start(*m_hbox2, Gtk::PACK_SHRINK, 0);
   m_vbox1->pack_start(*m_hbox1);
   m_vbox1->pack_start(*m_btnLaunch, Gtk::PACK_SHRINK, 0);
   wndMain->set_size_request(600,400);
   wndMain->set_title(_("VNC Proxy"));
   wndMain->set_modal(false);
   wndMain->property_window_position().set_value(Gtk::WIN_POS_NONE);
   wndMain->set_resizable(true);
   wndMain->property_destroy_with_parent().set_value(false);
   wndMain->add(*m_vbox1);
   m_lblTitle->show();
   m_label4->show();
   m_txtListenPort->show();
   m_label8->show();
   m_txtDesktopIP->show();
   m_label9->show();
   m_txtDesktopPort->show();
   m_hbox3->show();
   m_alignment1->show();
   m_label7->show();
   m_frame1->show();
   m_hbox2->show();
   m_lstAllServices->show();
   m_scrolledwindow1->show();
   m_alignment2->show();
   m_label2->show();
   m_frame2->show();
   m_btnAddServices->show();
   image1->show();
   label1->show();
   hbox1->show();
   alignment1->show();
   m_btnAddAllServices->show();
   m_btnRemoveServices->show();
   m_vbox2->show();
   m_lstSelectedServices->show();
   m_scrolledwindow2->show();
   m_alignment3->show();
   m_label3->show();
   m_frame3->show();
   m_hbox1->show();
   m_btnLaunch->show();
   m_vbox1->show();
   wndMain->show();
   m_txtListenPort->signal_changed().connect(SigC::slot(*this, &wndMain_glade::on_m_txtListenPort_changed), false);
   m_txtDesktopIP->signal_changed().connect(SigC::slot(*this, &wndMain_glade::on_m_txtDesktopIP_changed), false);
   m_txtDesktopPort->signal_changed().connect(SigC::slot(*this, &wndMain_glade::on_m_txtDesktopPort_changed), false);
   m_btnAddServices->signal_clicked().connect(SigC::slot(*this, &wndMain_glade::on_btnAddServices_clicked), false);
   m_btnAddAllServices->signal_clicked().connect(SigC::slot(*this, &wndMain_glade::on_m_btnAddAllServices_clicked), false);
   m_btnRemoveServices->signal_clicked().connect(SigC::slot(*this, &wndMain_glade::on_btnRemoveServices_clicked), false);
   m_btnLaunch->signal_clicked().connect(SigC::slot(*this, &wndMain_glade::on_btnLaunch_clicked), false);
}

wndMain_glade::~wndMain_glade()
{  delete gmm_data;
}
