/*
 * TorrentView.cpp
 *
 *  Created on: 29/05/2009
 *      Author: teddy
 */

#include "TorrentView.h"
#include <iomanip>

#define COL_NAME "Nombre del Torrent"
#define COL_SIZE "Tamaño"
#define COL_STATUS "Estado"
#define COL_PROGRESS "Progreso"
#define COL_COMPLETED "Completado"
#define COL_REMAINING "Restante"
#define COL_DOWNSPEED "Vel descarga (kb/s)"
#define COL_UPSPEED "Vel subida (kb/s)"
#define COL_TIME "Tiempo Restante"
#define COL_CAT "Mostrar"

#define SHOW_ALL "Todos"

TorrentView::TorrentView()
{
	//agrego las columnas
	columns.add(col_name);
	columns.add(col_size);
	columns.add(col_status);
	columns.add(col_progress);
	columns.add(col_completed);
	columns.add(col_remaining);
	columns.add(col_downspeed);
	columns.add(col_upspeed);
	columns.add(col_time);
	columns.add(col_torrent);
	columns.add(col_visible);

	columns_categories.add(col_categories);
	columns_categories.add(col_cat_status);

	//creo la lista con las columnas
	list_torrents = Gtk::ListStore::create(columns);
	list_categories = Gtk::ListStore::create(columns_categories);

	filter = Gtk::TreeModelFilter::create(list_torrents);
	filter->set_visible_column(col_visible);

	i = 0;
}

TorrentView::~TorrentView()
{
	list_torrents->clear();
	list_categories->clear();
}

void TorrentView::setTreeView(Gtk::TreeView *view_torrents)
{
	this->view_torrents = view_torrents;

	//agrego la lista de torrents al tree view
	this->view_torrents->set_model(list_torrents);

	Gtk::TreeViewColumn* pColumn;
	int cols_count;

	//agrego columnas al Tree View
	this->view_torrents->append_column(COL_NAME, col_name);
	this->view_torrents->append_column(COL_SIZE, col_size);
	this->view_torrents->append_column(COL_STATUS, col_status);

	//mostrar una progress bar para el porcentaje de progreso
	Gtk::CellRendererProgress* cell = Gtk::manage(new Gtk::CellRendererProgress);
	cols_count = this->view_torrents->append_column(COL_PROGRESS, *cell);
	pColumn = this->view_torrents->get_column(cols_count - 1);
 	if(pColumn)
 	{
#ifdef GLIBMM_PROPERTIES_ENABLED
 		pColumn->add_attribute(cell->property_value(), col_progress);
#else
 		pColumn->add_attribute(*cell, "value", col_progress);
#endif
 	}

	this->view_torrents->append_column(COL_COMPLETED, col_completed);
	this->view_torrents->append_column(COL_REMAINING, col_remaining);
	this->view_torrents->append_column(COL_DOWNSPEED, col_downspeed);
	this->view_torrents->append_column(COL_UPSPEED, col_upspeed);
	cols_count = this->view_torrents->append_column(COL_TIME, col_time);

	for (int i=0; i<cols_count; i++)
	{
		pColumn = this->view_torrents->get_column(i);
		pColumn->set_resizable(true);	//hago que sean columnas redimensionables
	}
	this->view_torrents->columns_autosize();

	selection = this->view_torrents->get_selection();
	selection->signal_changed().connect( sigc::mem_fun(*this, &TorrentView::on_row_selected) );

}

void TorrentView::setCategoriesView(Gtk::TreeView *view_categories)
{
	this->view_categories = view_categories;
	this->view_categories->set_model(list_categories);
	this->view_categories->append_column(COL_CAT, col_categories);

	//agrego filas a la lista
	Gtk::TreeModel::Row row = (* list_categories->append());
	row[col_categories] = "Todos";
	row[col_cat_status] = SHOW_ALL;

	row = (* list_categories->append());
	row[col_categories] = "Activos";
	row[col_cat_status] = T_ACTIVO;

	row = (* list_categories->append());
	row[col_categories] = "Completos";
	row[col_cat_status] = T_COMPLETO;

	row = (* list_categories->append());
	row[col_categories] = "Detenidos";
	row[col_cat_status] = T_DETENIDO;


	selection_categories = this->view_categories->get_selection();
	//me conecto a la señal de fila seleccionada
	selection_categories->signal_changed().connect( sigc::mem_fun(*this, &TorrentView::on_category_selected) );

}

void TorrentView::setAttributesView(AttributesView *attr)
{
	this->attr = attr;
}

void TorrentView::on_category_selected()
{
	//veo que fila esta seleccionada
	Gtk::TreeModel::iterator iter = selection_categories->get_selected();
	if (iter) //si hay algo seleccionado
	{
	  Gtk::TreeModel::Row row = *iter;
	  //veo que fila es y muestro solo los torrents en ese estado
	  std::string type = row[col_cat_status];
	  std::cout<<"categoria seleccionada: "<<type<<std::endl;
	  (type != SHOW_ALL) ? hideRows(type) : showHidden();
	}
}

void TorrentView::on_row_selected()
{
	std::cout<<"fila seleccionada"<<std::endl;
	//muestra informacion del torrent en la parte inferior de la pantalla
	Torrent *t = getSelectedTorrent();
	attr->showInfo(t);
}

void TorrentView::updateRowValues(Gtk::TreeModel::Row &row, Torrent *t)
{
	//todo sacar i!
	row[col_status] = t->getEstado();
	row[col_progress] = (t->getTamanioDescargado()+i) * 100 / t->getTamanio();
	row[col_completed] = showBytes(t->getTamanioDescargado() + i);
	//row[col_remaining] = showBytes(t->left());
	row[col_remaining] =showBytes(t->getTamanio() - (t->getTamanioDescargado()+i));
	row[col_downspeed] = t->getVelocidadBajada();
	row[col_upspeed] = t->getVelocidadSubida();
	//row[col_time] = t->getTiempoRestante();

	row[col_time] = "1h 25m";
}

void TorrentView::addRow(Torrent *t)
{
	Gtk::TreeModel::Row row = (* list_torrents->append());

	row[col_name] = t->getNombre();
	row[col_size] = showBytes(t->getTamanio());
	row[col_torrent] = t;
	row[col_visible] = true;
	//row[col_name] = t->getNombre();
	row[col_name] = "Nombre del Torrent";

	updateRowValues(row,t);
	i += 1000;
}

Torrent* TorrentView::getSelectedTorrent()
{
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //si hay algo seleccionado
	{
	  Gtk::TreeModel::Row row = *iter;
	  return row.get_value(col_torrent);
	}
	else
	{
		return NULL;
	}
}

void TorrentView::eraseSelectedRow()
{
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //si hay algo seleccionado
	{
	  iter = list_torrents->erase(iter); //borro y obtengo un iter a la sgte fila
	  if (iter)	//si existe la fila
		 selection->select(iter);	//selecciono la fila
	}

}

void TorrentView::selectNext()
{
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //si hay algo seleccionado
	{
	  iter++;
	  if(iter)		  //si existe la fila
	  {
		   selection->select(iter);	//selecciono la fila
		   Gtk::TreePath::TreePath path(iter);
		   view_torrents->scroll_to_row(path); //muevo el scroll de la vista
	  }
	}
}

void TorrentView::selectPrevious()
{
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if(iter) //si hay algo seleccionado
	{
		if (iter != list_torrents->children().begin())
		{
			iter--;	//retrocedo
			selection->select(iter);	//selecciono la fila
			Gtk::TreePath::TreePath path(iter);
			view_torrents->scroll_to_row(path); //muevo el scroll de la vista
		}
	}
}

void TorrentView::empty()
{
	list_torrents->clear();
}

void TorrentView::updateRow(Torrent *t)
{
	std::cout<<"__actualizar fila "<<std::endl;

	//busco la fila
	Gtk::TreeModel::Children::iterator iter = list_torrents->children().begin();
	std::cout<<"iter "<<std::endl;
	Gtk::TreeModel::Row row;
	while (iter != list_torrents->children().end())
	{
		row = *iter;
		if (row[col_torrent] == t)
		{
			std::cout<<"_____ update "<<std::endl;
			this->updateRowValues(row,t);
			break;
		}
		iter++;
	}
	std::cout<<"__fin actualizar "<<std::endl;
}

std::string TorrentView::showBytes(float bytes)
{
	std::stringstream buffer;
	if (bytes < 1024)
	{
		buffer << std::setprecision(3)<< bytes<<" bytes";
	}
	else
	{
		bytes /= 1024;
		if (bytes < 1024)
		{
			buffer << std::setprecision(3)<<bytes<<" kb";
		}
		else
		{
			bytes /= 1024;
			buffer << std::setprecision(3)<<bytes<<" Mb";
		}
	}
	return buffer.str();
}

void TorrentView::hideRows(std::string type)
{
	std::cout<<"mostrar "<<type<<"s"<<std::endl;
	Gtk::TreeModel::Children::iterator iter = list_torrents->children().begin();
	Gtk::TreeModel::Row row;
	Torrent *t;
	while (iter != list_torrents->children().end())
	{
		row = *iter;
		t = row[col_torrent];
		row[col_visible] = (type == t->getEstado());
		iter++;
	}
	filter->refilter();//ver si es necesario
}

void TorrentView::showHidden()
{
	std::cout<<"mostrar todos"<<std::endl;
	Gtk::TreeModel::Children::iterator iter = list_torrents->children().begin();
	Gtk::TreeModel::Row row;
	while (iter != list_torrents->children().end())
	{
		row = *iter;
		row[col_visible] = true;
		iter++;
	}
	filter->refilter();//ver si es necesario
}
