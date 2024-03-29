/*
 * TorrentView.cpp
 *
 *  Created on: 29/05/2009
 *      Author: Lucia
 */

#include "TorrentView.h"

#define COL_NAME "Nombre del Torrent"
#define COL_SIZE "Tamaño"
#define COL_STATUS "Estado"
#define COL_PROGRESS "Progreso"
#define COL_COMPLETED "Completado"
#define COL_REMAINING "Restante"
#define COL_DOWNSPEED "Vel descarga (kb/s)"
#define COL_UPSPEED "Vel subida (kb/s)"
#define COL_TIME "Tiempo Restante"
#define COL_CAT "Estados"
#define COL_CANT "#"

#define SHOW_ALL "Todos"

TorrentView::TorrentView() {

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

	columns_categories.add(col_categories);
	columns_categories.add(col_cat_status);
	columns_categories.add(col_cantidad);

	//creo la lista con las columnas
	list_torrents = Gtk::ListStore::create(columns);
	list_categories = Gtk::ListStore::create(columns_categories);

}

TorrentView::~TorrentView() {
	list_torrents->clear();
	list_categories->clear();
}

void TorrentView::setTreeView(Gtk::TreeView *view_torrents) {
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
	Gtk::CellRendererProgress* cell =
		Gtk::manage(new Gtk::CellRendererProgress);
	cols_count = this->view_torrents->append_column(COL_PROGRESS, *cell);
	pColumn = this->view_torrents->get_column(cols_count - 1);
	if (pColumn) {
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

	for (int i = 0; i < cols_count; i++) {
		pColumn = this->view_torrents->get_column(i);
		pColumn->set_resizable(true); //hago que sean columnas redimensionables
	}
	this->view_torrents->columns_autosize();

	selection = this->view_torrents->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this,
			&TorrentView::on_row_selected));

}

void TorrentView::setCategoriesView(Gtk::TreeView *view_categories) {
	this->view_categories = view_categories;
	this->view_categories->set_model(list_categories);
	this->view_categories->append_column(COL_CAT, col_categories);
	this->view_categories->append_column(COL_CANT, col_cantidad);

	//agrego filas a la lista
	Gtk::TreeModel::Row row = (*list_categories->append());
	row[col_categories] = "Todos";
	row[col_cat_status] = SHOW_ALL;
	row[col_cantidad] = 0;

	row = (*list_categories->append());
	row[col_categories] = "Activos";
	row[col_cat_status] = T_ACTIVO;
	row[col_cantidad] = 0;

	row = (*list_categories->append());
	row[col_categories] = "Completos";
	row[col_cat_status] = T_COMPLETO;
	row[col_cantidad] = 0;

	row = (*list_categories->append());
	row[col_categories] = "Detenidos";
	row[col_cat_status] = T_DETENIDO;
	row[col_cantidad] = 0;

	this->view_categories->columns_autosize();
}

void TorrentView::setAttributesView(AttributesView *attr) {
	this->attr = attr;
}

void TorrentView::on_row_selected() {
	Torrent *t = getSelectedTorrent();
	if (t != NULL)
		attr->showInfo(t);
}

void TorrentView::updateRowValues(Gtk::TreeModel::Row &row, Torrent *t) {
	updateCantidad(t->getEstado(), row[col_status]);
	row[col_status] = t->getEstado();
	row[col_remaining] = t->bytesToString(t->left());
	row[col_downspeed] = t->getVelocidadBajada();
	row[col_upspeed] = t->getVelocidadSubida();
	row[col_time] = t->getTiempoRestante();
	row[col_progress] = (int) (((t->getTamanioDescargado() / 1024) * 100)
			/ (t->getTamanio() / 1024));
	row[col_completed] = t->bytesToString(t->getTamanioDescargado());
}

void TorrentView::addRow(Torrent *t) {
	Gtk::TreeModel::Row row = (*list_torrents->append());

	row[col_name] = t->getNombre();
	row[col_size] = t->bytesToString(t->getTamanio());
	row[col_torrent] = t;

	updateRowValues(row, t);
}

Torrent* TorrentView::getSelectedTorrent() {
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (iter) {//si hay algo seleccionado
		Gtk::TreeModel::Row row = *iter;
		return row.get_value(col_torrent);
	} else {
		return NULL;
	}
}

void TorrentView::eraseSelectedRow() {
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (iter) { //si hay algo seleccionado
		iter = list_torrents->erase(iter); //borro y obtengo un iter a la sgte fila
		if (iter) //si existe la fila
			selection->select(iter); //selecciono la fila
	}
}

void TorrentView::selectNext() {
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (iter) {//si hay algo seleccionado

		iter++;
		if (iter) { //si existe la fila
			selection->select(iter); //selecciono la fila
			Gtk::TreePath::TreePath path(iter);
			view_torrents->scroll_to_row(path); //muevo el scroll de la vista
		}
	}
}

void TorrentView::selectPrevious() {
	Gtk::TreeModel::iterator iter = selection->get_selected();
	if (iter) {//si hay algo seleccionado
		if (iter != list_torrents->children().begin()) {
			iter--; //retrocedo
			selection->select(iter); //selecciono la fila
			Gtk::TreePath::TreePath path(iter);
			view_torrents->scroll_to_row(path); //muevo el scroll de la vista
		}
	}
}

void TorrentView::empty() {
	list_torrents->clear();
}

void TorrentView::updateRow(Torrent *t) {
	Gtk::TreeModel::Children::iterator iter = list_torrents->children().begin();
	Gtk::TreeModel::Row row;
	//busca la fila del torrent y actualiza sus valores
	while (iter != list_torrents->children().end()) {
		row = *iter;
		if (row[col_torrent] == t) {
			updateRowValues(row, t);
			break;
		}
		iter++;
	}
}

void TorrentView::updateCantidad(std::string estado, std::string anterior) {
	Gtk::TreeModel::Children::iterator iter = list_categories->children().begin();
	Gtk::TreeModel::Row row;
	UINT cant = 0, total = 0;
	std::string aux;
	while (iter != list_categories->children().end()) {
		row = *iter;
		aux = row[col_cat_status];
		if (aux == estado) {
			cant = row[col_cantidad];
			cant++;
			row[col_cantidad] = cant;
		}
		if (aux == anterior) {
			cant = row[col_cantidad];
			cant--;
			row[col_cantidad] = cant;
		}
		iter++;
		total += row[col_cantidad];
	}
	row = (*list_categories->children().begin());
	row[col_cantidad] = total - row[col_cantidad];
}
