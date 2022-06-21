/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
<<<<<<< HEAD
=======
 * Copyright (C) 2015 Boris Barbulovski <bbarbulovski@gmail.com>
>>>>>>> v4.9.227
 * Released under the terms of the GNU GPL v2.0.
 */

#include <qglobal.h>

<<<<<<< HEAD
#if QT_VERSION < 0x040000
#include <stddef.h>
#include <qmainwindow.h>
#include <qvbox.h>
#include <qvaluelist.h>
#include <qtextbrowser.h>
#include <qaction.h>
#include <qheader.h>
#include <qfiledialog.h>
#include <qdragobject.h>
#include <qpopupmenu.h>
#else
#include <q3mainwindow.h>
#include <q3vbox.h>
#include <q3valuelist.h>
#include <q3textbrowser.h>
#include <q3action.h>
#include <q3header.h>
#include <q3filedialog.h>
#include <q3dragobject.h>
#include <q3popupmenu.h>
#endif
=======
#include <QMainWindow>
#include <QList>
#include <qtextbrowser.h>
#include <QAction>
#include <QFileDialog>
#include <QMenu>
>>>>>>> v4.9.227

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qtoolbar.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qevent.h>

#include <stdlib.h>

#include "lkc.h"
#include "qconf.h"

#include "qconf.moc"
#include "images.c"

#ifdef _
# undef _
# define _ qgettext
#endif

static QApplication *configApp;
static ConfigSettings *configSettings;

<<<<<<< HEAD
Q3Action *ConfigMainWindow::saveAction;
=======
QAction *ConfigMainWindow::saveAction;
>>>>>>> v4.9.227

static inline QString qgettext(const char* str)
{
	return QString::fromLocal8Bit(gettext(str));
}

static inline QString qgettext(const QString& str)
{
<<<<<<< HEAD
	return QString::fromLocal8Bit(gettext(str.latin1()));
=======
	return QString::fromLocal8Bit(gettext(str.toLatin1()));
>>>>>>> v4.9.227
}

ConfigSettings::ConfigSettings()
	: QSettings("kernel.org", "qconf")
{
}

/**
 * Reads a list of integer values from the application settings.
 */
<<<<<<< HEAD
Q3ValueList<int> ConfigSettings::readSizes(const QString& key, bool *ok)
{
	Q3ValueList<int> result;
	QStringList entryList = readListEntry(key, ok);
=======
QList<int> ConfigSettings::readSizes(const QString& key, bool *ok)
{
	QList<int> result;
	QStringList entryList = value(key).toStringList();
>>>>>>> v4.9.227
	QStringList::Iterator it;

	for (it = entryList.begin(); it != entryList.end(); ++it)
		result.push_back((*it).toInt());

	return result;
}

/**
 * Writes a list of integer values to the application settings.
 */
<<<<<<< HEAD
bool ConfigSettings::writeSizes(const QString& key, const Q3ValueList<int>& value)
{
	QStringList stringList;
	Q3ValueList<int>::ConstIterator it;

	for (it = value.begin(); it != value.end(); ++it)
		stringList.push_back(QString::number(*it));
	return writeEntry(key, stringList);
=======
bool ConfigSettings::writeSizes(const QString& key, const QList<int>& value)
{
	QStringList stringList;
	QList<int>::ConstIterator it;

	for (it = value.begin(); it != value.end(); ++it)
		stringList.push_back(QString::number(*it));
	setValue(key, stringList);

	return true;
>>>>>>> v4.9.227
}


/*
 * set the new data
 * TODO check the value
 */
void ConfigItem::okRename(int col)
{
<<<<<<< HEAD
	Parent::okRename(col);
	sym_set_string_value(menu->sym, text(dataColIdx).latin1());
	listView()->updateList(this);
=======
>>>>>>> v4.9.227
}

/*
 * update the displayed of a menu entry
 */
void ConfigItem::updateMenu(void)
{
	ConfigList* list;
	struct symbol* sym;
	struct property *prop;
	QString prompt;
	int type;
	tristate expr;

	list = listView();
	if (goParent) {
		setPixmap(promptColIdx, list->menuBackPix);
		prompt = "..";
		goto set_prompt;
	}

	sym = menu->sym;
	prop = menu->prompt;
	prompt = _(menu_get_prompt(menu));

	if (prop) switch (prop->type) {
	case P_MENU:
		if (list->mode == singleMode || list->mode == symbolMode) {
			/* a menuconfig entry is displayed differently
			 * depending whether it's at the view root or a child.
			 */
			if (sym && list->rootEntry == menu)
				break;
			setPixmap(promptColIdx, list->menuPix);
		} else {
			if (sym)
				break;
<<<<<<< HEAD
			setPixmap(promptColIdx, 0);
		}
		goto set_prompt;
	case P_COMMENT:
		setPixmap(promptColIdx, 0);
=======
			setPixmap(promptColIdx, QIcon());
		}
		goto set_prompt;
	case P_COMMENT:
		setPixmap(promptColIdx, QIcon());
>>>>>>> v4.9.227
		goto set_prompt;
	default:
		;
	}
	if (!sym)
		goto set_prompt;

	setText(nameColIdx, QString::fromLocal8Bit(sym->name));

	type = sym_get_type(sym);
	switch (type) {
	case S_BOOLEAN:
	case S_TRISTATE:
		char ch;

		if (!sym_is_changable(sym) && list->optMode == normalOpt) {
<<<<<<< HEAD
			setPixmap(promptColIdx, 0);
=======
			setPixmap(promptColIdx, QIcon());
>>>>>>> v4.9.227
			setText(noColIdx, QString::null);
			setText(modColIdx, QString::null);
			setText(yesColIdx, QString::null);
			break;
		}
		expr = sym_get_tristate_value(sym);
		switch (expr) {
		case yes:
			if (sym_is_choice_value(sym) && type == S_BOOLEAN)
				setPixmap(promptColIdx, list->choiceYesPix);
			else
				setPixmap(promptColIdx, list->symbolYesPix);
			setText(yesColIdx, "Y");
			ch = 'Y';
			break;
		case mod:
			setPixmap(promptColIdx, list->symbolModPix);
			setText(modColIdx, "M");
			ch = 'M';
			break;
		default:
			if (sym_is_choice_value(sym) && type == S_BOOLEAN)
				setPixmap(promptColIdx, list->choiceNoPix);
			else
				setPixmap(promptColIdx, list->symbolNoPix);
			setText(noColIdx, "N");
			ch = 'N';
			break;
		}
		if (expr != no)
			setText(noColIdx, sym_tristate_within_range(sym, no) ? "_" : 0);
		if (expr != mod)
			setText(modColIdx, sym_tristate_within_range(sym, mod) ? "_" : 0);
		if (expr != yes)
			setText(yesColIdx, sym_tristate_within_range(sym, yes) ? "_" : 0);

		setText(dataColIdx, QChar(ch));
		break;
	case S_INT:
	case S_HEX:
	case S_STRING:
		const char* data;

		data = sym_get_string_value(sym);

<<<<<<< HEAD
		int i = list->mapIdx(dataColIdx);
		if (i >= 0)
			setRenameEnabled(i, TRUE);
=======
>>>>>>> v4.9.227
		setText(dataColIdx, data);
		if (type == S_STRING)
			prompt = QString("%1: %2").arg(prompt).arg(data);
		else
			prompt = QString("(%2) %1").arg(prompt).arg(data);
		break;
	}
	if (!sym_has_value(sym) && visible)
		prompt += _(" (NEW)");
set_prompt:
	setText(promptColIdx, prompt);
}

void ConfigItem::testUpdateMenu(bool v)
{
	ConfigItem* i;

	visible = v;
	if (!menu)
		return;

	sym_calc_value(menu->sym);
	if (menu->flags & MENU_CHANGED) {
		/* the menu entry changed, so update all list items */
		menu->flags &= ~MENU_CHANGED;
		for (i = (ConfigItem*)menu->data; i; i = i->nextItem)
			i->updateMenu();
	} else if (listView()->updateAll)
		updateMenu();
}

<<<<<<< HEAD
void ConfigItem::paintCell(QPainter* p, const QColorGroup& cg, int column, int width, int align)
{
	ConfigList* list = listView();

	if (visible) {
		if (isSelected() && !list->hasFocus() && list->mode == menuMode)
			Parent::paintCell(p, list->inactivedColorGroup, column, width, align);
		else
			Parent::paintCell(p, cg, column, width, align);
	} else
		Parent::paintCell(p, list->disabledColorGroup, column, width, align);
}
=======
>>>>>>> v4.9.227

/*
 * construct a menu entry
 */
void ConfigItem::init(void)
{
	if (menu) {
		ConfigList* list = listView();
		nextItem = (ConfigItem*)menu->data;
		menu->data = this;

		if (list->mode != fullMode)
<<<<<<< HEAD
			setOpen(TRUE);
=======
			setExpanded(true);
>>>>>>> v4.9.227
		sym_calc_value(menu->sym);
	}
	updateMenu();
}

/*
 * destruct a menu entry
 */
ConfigItem::~ConfigItem(void)
{
	if (menu) {
		ConfigItem** ip = (ConfigItem**)&menu->data;
		for (; *ip; ip = &(*ip)->nextItem) {
			if (*ip == this) {
				*ip = nextItem;
				break;
			}
		}
	}
}

ConfigLineEdit::ConfigLineEdit(ConfigView* parent)
	: Parent(parent)
{
<<<<<<< HEAD
	connect(this, SIGNAL(lostFocus()), SLOT(hide()));
=======
	connect(this, SIGNAL(editingFinished()), SLOT(hide()));
>>>>>>> v4.9.227
}

void ConfigLineEdit::show(ConfigItem* i)
{
	item = i;
	if (sym_get_string_value(item->menu->sym))
		setText(QString::fromLocal8Bit(sym_get_string_value(item->menu->sym)));
	else
		setText(QString::null);
	Parent::show();
	setFocus();
}

void ConfigLineEdit::keyPressEvent(QKeyEvent* e)
{
	switch (e->key()) {
	case Qt::Key_Escape:
		break;
	case Qt::Key_Return:
	case Qt::Key_Enter:
<<<<<<< HEAD
		sym_set_string_value(item->menu->sym, text().latin1());
=======
		sym_set_string_value(item->menu->sym, text().toLatin1());
>>>>>>> v4.9.227
		parent()->updateList(item);
		break;
	default:
		Parent::keyPressEvent(e);
		return;
	}
	e->accept();
	parent()->list->setFocus();
	hide();
}

ConfigList::ConfigList(ConfigView* p, const char *name)
<<<<<<< HEAD
	: Parent(p, name),
=======
	: Parent(p),
>>>>>>> v4.9.227
	  updateAll(false),
	  symbolYesPix(xpm_symbol_yes), symbolModPix(xpm_symbol_mod), symbolNoPix(xpm_symbol_no),
	  choiceYesPix(xpm_choice_yes), choiceNoPix(xpm_choice_no),
	  menuPix(xpm_menu), menuInvPix(xpm_menu_inv), menuBackPix(xpm_menuback), voidPix(xpm_void),
<<<<<<< HEAD
	  showName(false), showRange(false), showData(false), optMode(normalOpt),
=======
	  showName(false), showRange(false), showData(false), mode(singleMode), optMode(normalOpt),
>>>>>>> v4.9.227
	  rootEntry(0), headerPopup(0)
{
	int i;

<<<<<<< HEAD
	setSorting(-1);
	setRootIsDecorated(TRUE);
	disabledColorGroup = palette().active();
	disabledColorGroup.setColor(QColorGroup::Text, palette().disabled().text());
	inactivedColorGroup = palette().active();
	inactivedColorGroup.setColor(QColorGroup::Highlight, palette().disabled().highlight());

	connect(this, SIGNAL(selectionChanged(void)),
=======
	setObjectName(name);
	setSortingEnabled(false);
	setRootIsDecorated(true);

	setVerticalScrollMode(ScrollPerPixel);
	setHorizontalScrollMode(ScrollPerPixel);

	setHeaderLabels(QStringList() << _("Option") << _("Name") << "N" << "M" << "Y" << _("Value"));

	connect(this, SIGNAL(itemSelectionChanged(void)),
>>>>>>> v4.9.227
		SLOT(updateSelection(void)));

	if (name) {
		configSettings->beginGroup(name);
<<<<<<< HEAD
		showName = configSettings->readBoolEntry("/showName", false);
		showRange = configSettings->readBoolEntry("/showRange", false);
		showData = configSettings->readBoolEntry("/showData", false);
		optMode = (enum optionMode)configSettings->readNumEntry("/optionMode", false);
=======
		showName = configSettings->value("/showName", false).toBool();
		showRange = configSettings->value("/showRange", false).toBool();
		showData = configSettings->value("/showData", false).toBool();
		optMode = (enum optionMode)configSettings->value("/optionMode", 0).toInt();
>>>>>>> v4.9.227
		configSettings->endGroup();
		connect(configApp, SIGNAL(aboutToQuit()), SLOT(saveSettings()));
	}

<<<<<<< HEAD
	for (i = 0; i < colNr; i++)
		colMap[i] = colRevMap[i] = -1;
	addColumn(promptColIdx, _("Option"));
=======
	addColumn(promptColIdx);
>>>>>>> v4.9.227

	reinit();
}

bool ConfigList::menuSkip(struct menu *menu)
{
	if (optMode == normalOpt && menu_is_visible(menu))
		return false;
	if (optMode == promptOpt && menu_has_prompt(menu))
		return false;
	if (optMode == allOpt)
		return false;
	return true;
}

void ConfigList::reinit(void)
{
	removeColumn(dataColIdx);
	removeColumn(yesColIdx);
	removeColumn(modColIdx);
	removeColumn(noColIdx);
	removeColumn(nameColIdx);

	if (showName)
<<<<<<< HEAD
		addColumn(nameColIdx, _("Name"));
	if (showRange) {
		addColumn(noColIdx, "N");
		addColumn(modColIdx, "M");
		addColumn(yesColIdx, "Y");
	}
	if (showData)
		addColumn(dataColIdx, _("Value"));
=======
		addColumn(nameColIdx);
	if (showRange) {
		addColumn(noColIdx);
		addColumn(modColIdx);
		addColumn(yesColIdx);
	}
	if (showData)
		addColumn(dataColIdx);
>>>>>>> v4.9.227

	updateListAll();
}

void ConfigList::saveSettings(void)
{
<<<<<<< HEAD
	if (name()) {
		configSettings->beginGroup(name());
		configSettings->writeEntry("/showName", showName);
		configSettings->writeEntry("/showRange", showRange);
		configSettings->writeEntry("/showData", showData);
		configSettings->writeEntry("/optionMode", (int)optMode);
=======
	if (!objectName().isEmpty()) {
		configSettings->beginGroup(objectName());
		configSettings->setValue("/showName", showName);
		configSettings->setValue("/showRange", showRange);
		configSettings->setValue("/showData", showData);
		configSettings->setValue("/optionMode", (int)optMode);
>>>>>>> v4.9.227
		configSettings->endGroup();
	}
}

ConfigItem* ConfigList::findConfigItem(struct menu *menu)
{
	ConfigItem* item = (ConfigItem*)menu->data;

	for (; item; item = item->nextItem) {
		if (this == item->listView())
			break;
	}

	return item;
}

void ConfigList::updateSelection(void)
{
	struct menu *menu;
	enum prop_type type;

<<<<<<< HEAD
	ConfigItem* item = (ConfigItem*)selectedItem();
=======
	if (selectedItems().count() == 0)
		return;

	ConfigItem* item = (ConfigItem*)selectedItems().first();
>>>>>>> v4.9.227
	if (!item)
		return;

	menu = item->menu;
	emit menuChanged(menu);
	if (!menu)
		return;
	type = menu->prompt ? menu->prompt->type : P_UNKNOWN;
	if (mode == menuMode && type == P_MENU)
		emit menuSelected(menu);
}

void ConfigList::updateList(ConfigItem* item)
{
	ConfigItem* last = 0;

	if (!rootEntry) {
		if (mode != listMode)
			goto update;
<<<<<<< HEAD
		Q3ListViewItemIterator it(this);
		ConfigItem* item;

		for (; it.current(); ++it) {
			item = (ConfigItem*)it.current();
			if (!item->menu)
				continue;
			item->testUpdateMenu(menu_is_visible(item->menu));
=======
		QTreeWidgetItemIterator it(this);
		ConfigItem* item;

		while (*it) {
			item = (ConfigItem*)(*it);
			if (!item->menu)
				continue;
			item->testUpdateMenu(menu_is_visible(item->menu));

			++it;
>>>>>>> v4.9.227
		}
		return;
	}

	if (rootEntry != &rootmenu && (mode == singleMode ||
	    (mode == symbolMode && rootEntry->parent != &rootmenu))) {
<<<<<<< HEAD
		item = firstChild();
=======
		item = (ConfigItem *)topLevelItem(0);
>>>>>>> v4.9.227
		if (!item)
			item = new ConfigItem(this, 0, true);
		last = item;
	}
	if ((mode == singleMode || (mode == symbolMode && !(rootEntry->flags & MENU_ROOT))) &&
	    rootEntry->sym && rootEntry->prompt) {
		item = last ? last->nextSibling() : firstChild();
		if (!item)
			item = new ConfigItem(this, last, rootEntry, true);
		else
			item->testUpdateMenu(true);

		updateMenuList(item, rootEntry);
<<<<<<< HEAD
		triggerUpdate();
=======
		update();
		resizeColumnToContents(0);
>>>>>>> v4.9.227
		return;
	}
update:
	updateMenuList(this, rootEntry);
<<<<<<< HEAD
	triggerUpdate();
=======
	update();
	resizeColumnToContents(0);
>>>>>>> v4.9.227
}

void ConfigList::setValue(ConfigItem* item, tristate val)
{
	struct symbol* sym;
	int type;
	tristate oldval;

	sym = item->menu ? item->menu->sym : 0;
	if (!sym)
		return;

	type = sym_get_type(sym);
	switch (type) {
	case S_BOOLEAN:
	case S_TRISTATE:
		oldval = sym_get_tristate_value(sym);

		if (!sym_set_tristate_value(sym, val))
			return;
		if (oldval == no && item->menu->list)
<<<<<<< HEAD
			item->setOpen(TRUE);
=======
			item->setExpanded(true);
>>>>>>> v4.9.227
		parent()->updateList(item);
		break;
	}
}

void ConfigList::changeValue(ConfigItem* item)
{
	struct symbol* sym;
	struct menu* menu;
	int type, oldexpr, newexpr;

	menu = item->menu;
	if (!menu)
		return;
	sym = menu->sym;
	if (!sym) {
		if (item->menu->list)
<<<<<<< HEAD
			item->setOpen(!item->isOpen());
=======
			item->setExpanded(!item->isExpanded());
>>>>>>> v4.9.227
		return;
	}

	type = sym_get_type(sym);
	switch (type) {
	case S_BOOLEAN:
	case S_TRISTATE:
		oldexpr = sym_get_tristate_value(sym);
		newexpr = sym_toggle_tristate_value(sym);
		if (item->menu->list) {
			if (oldexpr == newexpr)
<<<<<<< HEAD
				item->setOpen(!item->isOpen());
			else if (oldexpr == no)
				item->setOpen(TRUE);
=======
				item->setExpanded(!item->isExpanded());
			else if (oldexpr == no)
				item->setExpanded(true);
>>>>>>> v4.9.227
		}
		if (oldexpr != newexpr)
			parent()->updateList(item);
		break;
	case S_INT:
	case S_HEX:
	case S_STRING:
<<<<<<< HEAD
		if (colMap[dataColIdx] >= 0)
			item->startRename(colMap[dataColIdx]);
		else
			parent()->lineEdit->show(item);
=======
		parent()->lineEdit->show(item);
>>>>>>> v4.9.227
		break;
	}
}

void ConfigList::setRootMenu(struct menu *menu)
{
	enum prop_type type;

	if (rootEntry == menu)
		return;
	type = menu && menu->prompt ? menu->prompt->type : P_UNKNOWN;
	if (type != P_MENU)
		return;
	updateMenuList(this, 0);
	rootEntry = menu;
	updateListAll();
<<<<<<< HEAD
	setSelected(currentItem(), hasFocus());
	ensureItemVisible(currentItem());
=======
	if (currentItem()) {
		currentItem()->setSelected(hasFocus());
		scrollToItem(currentItem());
	}
>>>>>>> v4.9.227
}

void ConfigList::setParentMenu(void)
{
	ConfigItem* item;
	struct menu *oldroot;

	oldroot = rootEntry;
	if (rootEntry == &rootmenu)
		return;
	setRootMenu(menu_get_parent_menu(rootEntry->parent));

<<<<<<< HEAD
	Q3ListViewItemIterator it(this);
	for (; (item = (ConfigItem*)it.current()); it++) {
		if (item->menu == oldroot) {
			setCurrentItem(item);
			ensureItemVisible(item);
			break;
		}
=======
	QTreeWidgetItemIterator it(this);
	while (*it) {
		item = (ConfigItem *)(*it);
		if (item->menu == oldroot) {
			setCurrentItem(item);
			scrollToItem(item);
			break;
		}

		++it;
>>>>>>> v4.9.227
	}
}

/*
 * update all the children of a menu entry
 *   removes/adds the entries from the parent widget as necessary
 *
 * parent: either the menu list widget or a menu entry widget
 * menu: entry to be updated
 */
<<<<<<< HEAD
template <class P>
void ConfigList::updateMenuList(P* parent, struct menu* menu)
=======
void ConfigList::updateMenuList(ConfigItem *parent, struct menu* menu)
>>>>>>> v4.9.227
{
	struct menu* child;
	ConfigItem* item;
	ConfigItem* last;
	bool visible;
	enum prop_type type;

	if (!menu) {
<<<<<<< HEAD
		while ((item = parent->firstChild()))
			delete item;
=======
		while (parent->childCount() > 0)
		{
			delete parent->takeChild(0);
		}

>>>>>>> v4.9.227
		return;
	}

	last = parent->firstChild();
	if (last && !last->goParent)
		last = 0;
	for (child = menu->list; child; child = child->next) {
		item = last ? last->nextSibling() : parent->firstChild();
		type = child->prompt ? child->prompt->type : P_UNKNOWN;

		switch (mode) {
		case menuMode:
			if (!(child->flags & MENU_ROOT))
				goto hide;
			break;
		case symbolMode:
			if (child->flags & MENU_ROOT)
				goto hide;
			break;
		default:
			break;
		}

		visible = menu_is_visible(child);
		if (!menuSkip(child)) {
			if (!child->sym && !child->list && !child->prompt)
				continue;
			if (!item || item->menu != child)
				item = new ConfigItem(parent, last, child, visible);
			else
				item->testUpdateMenu(visible);

			if (mode == fullMode || mode == menuMode || type != P_MENU)
				updateMenuList(item, child);
			else
				updateMenuList(item, 0);
			last = item;
			continue;
		}
	hide:
		if (item && item->menu == child) {
			last = parent->firstChild();
			if (last == item)
				last = 0;
			else while (last->nextSibling() != item)
				last = last->nextSibling();
			delete item;
		}
	}
}

<<<<<<< HEAD
void ConfigList::keyPressEvent(QKeyEvent* ev)
{
	Q3ListViewItem* i = currentItem();
=======
void ConfigList::updateMenuList(ConfigList *parent, struct menu* menu)
{
	struct menu* child;
	ConfigItem* item;
	ConfigItem* last;
	bool visible;
	enum prop_type type;

	if (!menu) {
		while (parent->topLevelItemCount() > 0)
		{
			delete parent->takeTopLevelItem(0);
		}

		return;
	}

	last = (ConfigItem*)parent->topLevelItem(0);
	if (last && !last->goParent)
		last = 0;
	for (child = menu->list; child; child = child->next) {
		item = last ? last->nextSibling() : (ConfigItem*)parent->topLevelItem(0);
		type = child->prompt ? child->prompt->type : P_UNKNOWN;

		switch (mode) {
		case menuMode:
			if (!(child->flags & MENU_ROOT))
				goto hide;
			break;
		case symbolMode:
			if (child->flags & MENU_ROOT)
				goto hide;
			break;
		default:
			break;
		}

		visible = menu_is_visible(child);
		if (!menuSkip(child)) {
			if (!child->sym && !child->list && !child->prompt)
				continue;
			if (!item || item->menu != child)
				item = new ConfigItem(parent, last, child, visible);
			else
				item->testUpdateMenu(visible);

			if (mode == fullMode || mode == menuMode || type != P_MENU)
				updateMenuList(item, child);
			else
				updateMenuList(item, 0);
			last = item;
			continue;
		}
	hide:
		if (item && item->menu == child) {
			last = (ConfigItem*)parent->topLevelItem(0);
			if (last == item)
				last = 0;
			else while (last->nextSibling() != item)
				last = last->nextSibling();
			delete item;
		}
	}
}

void ConfigList::keyPressEvent(QKeyEvent* ev)
{
	QTreeWidgetItem* i = currentItem();
>>>>>>> v4.9.227
	ConfigItem* item;
	struct menu *menu;
	enum prop_type type;

	if (ev->key() == Qt::Key_Escape && mode != fullMode && mode != listMode) {
		emit parentSelected();
		ev->accept();
		return;
	}

	if (!i) {
		Parent::keyPressEvent(ev);
		return;
	}
	item = (ConfigItem*)i;

	switch (ev->key()) {
	case Qt::Key_Return:
	case Qt::Key_Enter:
		if (item->goParent) {
			emit parentSelected();
			break;
		}
		menu = item->menu;
		if (!menu)
			break;
		type = menu->prompt ? menu->prompt->type : P_UNKNOWN;
		if (type == P_MENU && rootEntry != menu &&
		    mode != fullMode && mode != menuMode) {
			emit menuSelected(menu);
			break;
		}
	case Qt::Key_Space:
		changeValue(item);
		break;
	case Qt::Key_N:
		setValue(item, no);
		break;
	case Qt::Key_M:
		setValue(item, mod);
		break;
	case Qt::Key_Y:
		setValue(item, yes);
		break;
	default:
		Parent::keyPressEvent(ev);
		return;
	}
	ev->accept();
}

<<<<<<< HEAD
void ConfigList::contentsMousePressEvent(QMouseEvent* e)
{
	//QPoint p(contentsToViewport(e->pos()));
	//printf("contentsMousePressEvent: %d,%d\n", p.x(), p.y());
	Parent::contentsMousePressEvent(e);
}

void ConfigList::contentsMouseReleaseEvent(QMouseEvent* e)
{
	QPoint p(contentsToViewport(e->pos()));
	ConfigItem* item = (ConfigItem*)itemAt(p);
	struct menu *menu;
	enum prop_type ptype;
	const QPixmap* pm;
=======
void ConfigList::mousePressEvent(QMouseEvent* e)
{
	//QPoint p(contentsToViewport(e->pos()));
	//printf("contentsMousePressEvent: %d,%d\n", p.x(), p.y());
	Parent::mousePressEvent(e);
}

void ConfigList::mouseReleaseEvent(QMouseEvent* e)
{
	QPoint p = e->pos();
	ConfigItem* item = (ConfigItem*)itemAt(p);
	struct menu *menu;
	enum prop_type ptype;
	QIcon icon;
>>>>>>> v4.9.227
	int idx, x;

	if (!item)
		goto skip;

	menu = item->menu;
	x = header()->offset() + p.x();
<<<<<<< HEAD
	idx = colRevMap[header()->sectionAt(x)];
	switch (idx) {
	case promptColIdx:
		pm = item->pixmap(promptColIdx);
		if (pm) {
			int off = header()->sectionPos(0) + itemMargin() +
				treeStepSize() * (item->depth() + (rootIsDecorated() ? 1 : 0));
			if (x >= off && x < off + pm->width()) {
=======
	idx = header()->logicalIndexAt(x);
	switch (idx) {
	case promptColIdx:
		icon = item->pixmap(promptColIdx);
		if (!icon.isNull()) {
			int off = header()->sectionPosition(0) + visualRect(indexAt(p)).x() + 4; // 4 is Hardcoded image offset. There might be a way to do it properly.
			if (x >= off && x < off + icon.availableSizes().first().width()) {
>>>>>>> v4.9.227
				if (item->goParent) {
					emit parentSelected();
					break;
				} else if (!menu)
					break;
				ptype = menu->prompt ? menu->prompt->type : P_UNKNOWN;
				if (ptype == P_MENU && rootEntry != menu &&
				    mode != fullMode && mode != menuMode)
					emit menuSelected(menu);
				else
					changeValue(item);
			}
		}
		break;
	case noColIdx:
		setValue(item, no);
		break;
	case modColIdx:
		setValue(item, mod);
		break;
	case yesColIdx:
		setValue(item, yes);
		break;
	case dataColIdx:
		changeValue(item);
		break;
	}

skip:
	//printf("contentsMouseReleaseEvent: %d,%d\n", p.x(), p.y());
<<<<<<< HEAD
	Parent::contentsMouseReleaseEvent(e);
}

void ConfigList::contentsMouseMoveEvent(QMouseEvent* e)
{
	//QPoint p(contentsToViewport(e->pos()));
	//printf("contentsMouseMoveEvent: %d,%d\n", p.x(), p.y());
	Parent::contentsMouseMoveEvent(e);
}

void ConfigList::contentsMouseDoubleClickEvent(QMouseEvent* e)
{
	QPoint p(contentsToViewport(e->pos()));
=======
	Parent::mouseReleaseEvent(e);
}

void ConfigList::mouseMoveEvent(QMouseEvent* e)
{
	//QPoint p(contentsToViewport(e->pos()));
	//printf("contentsMouseMoveEvent: %d,%d\n", p.x(), p.y());
	Parent::mouseMoveEvent(e);
}

void ConfigList::mouseDoubleClickEvent(QMouseEvent* e)
{
	QPoint p = e->pos(); // TODO: Check if this works(was contentsToViewport).
>>>>>>> v4.9.227
	ConfigItem* item = (ConfigItem*)itemAt(p);
	struct menu *menu;
	enum prop_type ptype;

	if (!item)
		goto skip;
	if (item->goParent) {
		emit parentSelected();
		goto skip;
	}
	menu = item->menu;
	if (!menu)
		goto skip;
	ptype = menu->prompt ? menu->prompt->type : P_UNKNOWN;
	if (ptype == P_MENU && (mode == singleMode || mode == symbolMode))
		emit menuSelected(menu);
	else if (menu->sym)
		changeValue(item);

skip:
	//printf("contentsMouseDoubleClickEvent: %d,%d\n", p.x(), p.y());
<<<<<<< HEAD
	Parent::contentsMouseDoubleClickEvent(e);
=======
	Parent::mouseDoubleClickEvent(e);
>>>>>>> v4.9.227
}

void ConfigList::focusInEvent(QFocusEvent *e)
{
	struct menu *menu = NULL;

	Parent::focusInEvent(e);

	ConfigItem* item = (ConfigItem *)currentItem();
	if (item) {
<<<<<<< HEAD
		setSelected(item, TRUE);
=======
		item->setSelected(true);
>>>>>>> v4.9.227
		menu = item->menu;
	}
	emit gotFocus(menu);
}

void ConfigList::contextMenuEvent(QContextMenuEvent *e)
{
	if (e->y() <= header()->geometry().bottom()) {
		if (!headerPopup) {
<<<<<<< HEAD
			Q3Action *action;

			headerPopup = new Q3PopupMenu(this);
			action = new Q3Action(NULL, _("Show Name"), 0, this);
			  action->setToggleAction(TRUE);
=======
			QAction *action;

			headerPopup = new QMenu(this);
			action = new QAction(_("Show Name"), this);
			  action->setCheckable(true);
>>>>>>> v4.9.227
			  connect(action, SIGNAL(toggled(bool)),
				  parent(), SLOT(setShowName(bool)));
			  connect(parent(), SIGNAL(showNameChanged(bool)),
				  action, SLOT(setOn(bool)));
<<<<<<< HEAD
			  action->setOn(showName);
			  action->addTo(headerPopup);
			action = new Q3Action(NULL, _("Show Range"), 0, this);
			  action->setToggleAction(TRUE);
=======
			  action->setChecked(showName);
			  headerPopup->addAction(action);
			action = new QAction(_("Show Range"), this);
			  action->setCheckable(true);
>>>>>>> v4.9.227
			  connect(action, SIGNAL(toggled(bool)),
				  parent(), SLOT(setShowRange(bool)));
			  connect(parent(), SIGNAL(showRangeChanged(bool)),
				  action, SLOT(setOn(bool)));
<<<<<<< HEAD
			  action->setOn(showRange);
			  action->addTo(headerPopup);
			action = new Q3Action(NULL, _("Show Data"), 0, this);
			  action->setToggleAction(TRUE);
=======
			  action->setChecked(showRange);
			  headerPopup->addAction(action);
			action = new QAction(_("Show Data"), this);
			  action->setCheckable(true);
>>>>>>> v4.9.227
			  connect(action, SIGNAL(toggled(bool)),
				  parent(), SLOT(setShowData(bool)));
			  connect(parent(), SIGNAL(showDataChanged(bool)),
				  action, SLOT(setOn(bool)));
<<<<<<< HEAD
			  action->setOn(showData);
			  action->addTo(headerPopup);
=======
			  action->setChecked(showData);
			  headerPopup->addAction(action);
>>>>>>> v4.9.227
		}
		headerPopup->exec(e->globalPos());
		e->accept();
	} else
		e->ignore();
}

ConfigView*ConfigView::viewList;
QAction *ConfigView::showNormalAction;
QAction *ConfigView::showAllAction;
QAction *ConfigView::showPromptAction;

ConfigView::ConfigView(QWidget* parent, const char *name)
<<<<<<< HEAD
	: Parent(parent, name)
{
	list = new ConfigList(this, name);
	lineEdit = new ConfigLineEdit(this);
	lineEdit->hide();
=======
	: Parent(parent)
{
	setObjectName(name);
	QVBoxLayout *verticalLayout = new QVBoxLayout(this);
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	list = new ConfigList(this);
	verticalLayout->addWidget(list);
	lineEdit = new ConfigLineEdit(this);
	lineEdit->hide();
	verticalLayout->addWidget(lineEdit);
>>>>>>> v4.9.227

	this->nextView = viewList;
	viewList = this;
}

ConfigView::~ConfigView(void)
{
	ConfigView** vp;

	for (vp = &viewList; *vp; vp = &(*vp)->nextView) {
		if (*vp == this) {
			*vp = nextView;
			break;
		}
	}
}

void ConfigView::setOptionMode(QAction *act)
{
	if (act == showNormalAction)
		list->optMode = normalOpt;
	else if (act == showAllAction)
		list->optMode = allOpt;
	else
		list->optMode = promptOpt;

	list->updateListAll();
}

void ConfigView::setShowName(bool b)
{
	if (list->showName != b) {
		list->showName = b;
		list->reinit();
		emit showNameChanged(b);
	}
}

void ConfigView::setShowRange(bool b)
{
	if (list->showRange != b) {
		list->showRange = b;
		list->reinit();
		emit showRangeChanged(b);
	}
}

void ConfigView::setShowData(bool b)
{
	if (list->showData != b) {
		list->showData = b;
		list->reinit();
		emit showDataChanged(b);
	}
}

void ConfigList::setAllOpen(bool open)
{
<<<<<<< HEAD
	Q3ListViewItemIterator it(this);

	for (; it.current(); it++)
		it.current()->setOpen(open);
=======
	QTreeWidgetItemIterator it(this);

	while (*it) {
		(*it)->setExpanded(open);

		++it;
	}
>>>>>>> v4.9.227
}

void ConfigView::updateList(ConfigItem* item)
{
	ConfigView* v;

	for (v = viewList; v; v = v->nextView)
		v->list->updateList(item);
}

void ConfigView::updateListAll(void)
{
	ConfigView* v;

	for (v = viewList; v; v = v->nextView)
		v->list->updateListAll();
}

ConfigInfoView::ConfigInfoView(QWidget* parent, const char *name)
<<<<<<< HEAD
	: Parent(parent, name), sym(0), _menu(0)
{
	if (name) {
		configSettings->beginGroup(name);
		_showDebug = configSettings->readBoolEntry("/showDebug", false);
=======
	: Parent(parent), sym(0), _menu(0)
{
	setObjectName(name);


	if (!objectName().isEmpty()) {
		configSettings->beginGroup(objectName());
		_showDebug = configSettings->value("/showDebug", false).toBool();
>>>>>>> v4.9.227
		configSettings->endGroup();
		connect(configApp, SIGNAL(aboutToQuit()), SLOT(saveSettings()));
	}
}

void ConfigInfoView::saveSettings(void)
{
<<<<<<< HEAD
	if (name()) {
		configSettings->beginGroup(name());
		configSettings->writeEntry("/showDebug", showDebug());
=======
	if (!objectName().isEmpty()) {
		configSettings->beginGroup(objectName());
		configSettings->setValue("/showDebug", showDebug());
>>>>>>> v4.9.227
		configSettings->endGroup();
	}
}

void ConfigInfoView::setShowDebug(bool b)
{
	if (_showDebug != b) {
		_showDebug = b;
		if (_menu)
			menuInfo();
		else if (sym)
			symbolInfo();
		emit showDebugChanged(b);
	}
}

void ConfigInfoView::setInfo(struct menu *m)
{
	if (_menu == m)
		return;
	_menu = m;
	sym = NULL;
	if (!_menu)
		clear();
	else
		menuInfo();
}

void ConfigInfoView::symbolInfo(void)
{
	QString str;

	str += "<big>Symbol: <b>";
	str += print_filter(sym->name);
	str += "</b></big><br><br>value: ";
	str += print_filter(sym_get_string_value(sym));
	str += "<br>visibility: ";
	str += sym->visible == yes ? "y" : sym->visible == mod ? "m" : "n";
	str += "<br>";
	str += debug_info(sym);

	setText(str);
}

void ConfigInfoView::menuInfo(void)
{
	struct symbol* sym;
	QString head, debug, help;

	sym = _menu->sym;
	if (sym) {
		if (_menu->prompt) {
			head += "<big><b>";
			head += print_filter(_(_menu->prompt->text));
			head += "</b></big>";
			if (sym->name) {
				head += " (";
				if (showDebug())
					head += QString().sprintf("<a href=\"s%p\">", sym);
				head += print_filter(sym->name);
				if (showDebug())
					head += "</a>";
				head += ")";
			}
		} else if (sym->name) {
			head += "<big><b>";
			if (showDebug())
				head += QString().sprintf("<a href=\"s%p\">", sym);
			head += print_filter(sym->name);
			if (showDebug())
				head += "</a>";
			head += "</b></big>";
		}
		head += "<br><br>";

		if (showDebug())
			debug = debug_info(sym);

		struct gstr help_gstr = str_new();
		menu_get_ext_help(_menu, &help_gstr);
		help = print_filter(str_get(&help_gstr));
		str_free(&help_gstr);
	} else if (_menu->prompt) {
		head += "<big><b>";
		head += print_filter(_(_menu->prompt->text));
		head += "</b></big><br><br>";
		if (showDebug()) {
			if (_menu->prompt->visible.expr) {
				debug += "&nbsp;&nbsp;dep: ";
				expr_print(_menu->prompt->visible.expr, expr_print_help, &debug, E_NONE);
				debug += "<br><br>";
			}
		}
	}
	if (showDebug())
		debug += QString().sprintf("defined at %s:%d<br><br>", _menu->file->name, _menu->lineno);

	setText(head + debug + help);
}

QString ConfigInfoView::debug_info(struct symbol *sym)
{
	QString debug;

	debug += "type: ";
	debug += print_filter(sym_type_name(sym->type));
	if (sym_is_choice(sym))
		debug += " (choice)";
	debug += "<br>";
	if (sym->rev_dep.expr) {
		debug += "reverse dep: ";
		expr_print(sym->rev_dep.expr, expr_print_help, &debug, E_NONE);
		debug += "<br>";
	}
	for (struct property *prop = sym->prop; prop; prop = prop->next) {
		switch (prop->type) {
		case P_PROMPT:
		case P_MENU:
			debug += QString().sprintf("prompt: <a href=\"m%p\">", prop->menu);
			debug += print_filter(_(prop->text));
			debug += "</a><br>";
			break;
		case P_DEFAULT:
		case P_SELECT:
		case P_RANGE:
		case P_ENV:
			debug += prop_get_type_name(prop->type);
			debug += ": ";
			expr_print(prop->expr, expr_print_help, &debug, E_NONE);
			debug += "<br>";
			break;
		case P_CHOICE:
			if (sym_is_choice(sym)) {
				debug += "choice: ";
				expr_print(prop->expr, expr_print_help, &debug, E_NONE);
				debug += "<br>";
			}
			break;
		default:
			debug += "unknown property: ";
			debug += prop_get_type_name(prop->type);
			debug += "<br>";
		}
		if (prop->visible.expr) {
			debug += "&nbsp;&nbsp;&nbsp;&nbsp;dep: ";
			expr_print(prop->visible.expr, expr_print_help, &debug, E_NONE);
			debug += "<br>";
		}
	}
	debug += "<br>";

	return debug;
}

QString ConfigInfoView::print_filter(const QString &str)
{
	QRegExp re("[<>&\"\\n]");
	QString res = str;
<<<<<<< HEAD
	for (int i = 0; (i = res.find(re, i)) >= 0;) {
		switch (res[i].latin1()) {
=======
	for (int i = 0; (i = res.indexOf(re, i)) >= 0;) {
		switch (res[i].toLatin1()) {
>>>>>>> v4.9.227
		case '<':
			res.replace(i, 1, "&lt;");
			i += 4;
			break;
		case '>':
			res.replace(i, 1, "&gt;");
			i += 4;
			break;
		case '&':
			res.replace(i, 1, "&amp;");
			i += 5;
			break;
		case '"':
			res.replace(i, 1, "&quot;");
			i += 6;
			break;
		case '\n':
			res.replace(i, 1, "<br>");
			i += 4;
			break;
		}
	}
	return res;
}

void ConfigInfoView::expr_print_help(void *data, struct symbol *sym, const char *str)
{
	QString* text = reinterpret_cast<QString*>(data);
	QString str2 = print_filter(str);

	if (sym && sym->name && !(sym->flags & SYMBOL_CONST)) {
		*text += QString().sprintf("<a href=\"s%p\">", sym);
		*text += str2;
		*text += "</a>";
	} else
		*text += str2;
}

<<<<<<< HEAD
Q3PopupMenu* ConfigInfoView::createPopupMenu(const QPoint& pos)
{
	Q3PopupMenu* popup = Parent::createPopupMenu(pos);
	Q3Action* action = new Q3Action(NULL, _("Show Debug Info"), 0, popup);
	  action->setToggleAction(TRUE);
	  connect(action, SIGNAL(toggled(bool)), SLOT(setShowDebug(bool)));
	  connect(this, SIGNAL(showDebugChanged(bool)), action, SLOT(setOn(bool)));
	  action->setOn(showDebug());
	popup->insertSeparator();
	action->addTo(popup);
	return popup;
}

void ConfigInfoView::contentsContextMenuEvent(QContextMenuEvent *e)
{
	Parent::contentsContextMenuEvent(e);
}

ConfigSearchWindow::ConfigSearchWindow(ConfigMainWindow* parent, const char *name)
	: Parent(parent, name), result(NULL)
{
	setCaption("Search Config");

	QVBoxLayout* layout1 = new QVBoxLayout(this, 11, 6);
	QHBoxLayout* layout2 = new QHBoxLayout(0, 0, 6);
=======
QMenu* ConfigInfoView::createStandardContextMenu(const QPoint & pos)
{
	QMenu* popup = Parent::createStandardContextMenu(pos);
	QAction* action = new QAction(_("Show Debug Info"), popup);
	  action->setCheckable(true);
	  connect(action, SIGNAL(toggled(bool)), SLOT(setShowDebug(bool)));
	  connect(this, SIGNAL(showDebugChanged(bool)), action, SLOT(setOn(bool)));
	  action->setChecked(showDebug());
	popup->addSeparator();
	popup->addAction(action);
	return popup;
}

void ConfigInfoView::contextMenuEvent(QContextMenuEvent *e)
{
	Parent::contextMenuEvent(e);
}

ConfigSearchWindow::ConfigSearchWindow(ConfigMainWindow* parent, const char *name)
	: Parent(parent), result(NULL)
{
	setObjectName(name);
	setWindowTitle("Search Config");

	QVBoxLayout* layout1 = new QVBoxLayout(this);
	layout1->setContentsMargins(11, 11, 11, 11);
	layout1->setSpacing(6);
	QHBoxLayout* layout2 = new QHBoxLayout(0);
	layout2->setContentsMargins(0, 0, 0, 0);
	layout2->setSpacing(6);
>>>>>>> v4.9.227
	layout2->addWidget(new QLabel(_("Find:"), this));
	editField = new QLineEdit(this);
	connect(editField, SIGNAL(returnPressed()), SLOT(search()));
	layout2->addWidget(editField);
	searchButton = new QPushButton(_("Search"), this);
<<<<<<< HEAD
	searchButton->setAutoDefault(FALSE);
=======
	searchButton->setAutoDefault(false);
>>>>>>> v4.9.227
	connect(searchButton, SIGNAL(clicked()), SLOT(search()));
	layout2->addWidget(searchButton);
	layout1->addLayout(layout2);

	split = new QSplitter(this);
	split->setOrientation(Qt::Vertical);
	list = new ConfigView(split, name);
	list->list->mode = listMode;
	info = new ConfigInfoView(split, name);
	connect(list->list, SIGNAL(menuChanged(struct menu *)),
		info, SLOT(setInfo(struct menu *)));
	connect(list->list, SIGNAL(menuChanged(struct menu *)),
		parent, SLOT(setMenuLink(struct menu *)));

	layout1->addWidget(split);

	if (name) {
<<<<<<< HEAD
		int x, y, width, height;
		bool ok;

		configSettings->beginGroup(name);
		width = configSettings->readNumEntry("/window width", parent->width() / 2);
		height = configSettings->readNumEntry("/window height", parent->height() / 2);
		resize(width, height);
		x = configSettings->readNumEntry("/window x", 0, &ok);
		if (ok)
			y = configSettings->readNumEntry("/window y", 0, &ok);
		if (ok)
			move(x, y);
		Q3ValueList<int> sizes = configSettings->readSizes("/split", &ok);
=======
		QVariant x, y;
		int width, height;
		bool ok;

		configSettings->beginGroup(name);
		width = configSettings->value("/window width", parent->width() / 2).toInt();
		height = configSettings->value("/window height", parent->height() / 2).toInt();
		resize(width, height);
		x = configSettings->value("/window x");
		y = configSettings->value("/window y");
		if ((x.isValid())&&(y.isValid()))
			move(x.toInt(), y.toInt());
		QList<int> sizes = configSettings->readSizes("/split", &ok);
>>>>>>> v4.9.227
		if (ok)
			split->setSizes(sizes);
		configSettings->endGroup();
		connect(configApp, SIGNAL(aboutToQuit()), SLOT(saveSettings()));
	}
}

void ConfigSearchWindow::saveSettings(void)
{
<<<<<<< HEAD
	if (name()) {
		configSettings->beginGroup(name());
		configSettings->writeEntry("/window x", pos().x());
		configSettings->writeEntry("/window y", pos().y());
		configSettings->writeEntry("/window width", size().width());
		configSettings->writeEntry("/window height", size().height());
=======
	if (!objectName().isEmpty()) {
		configSettings->beginGroup(objectName());
		configSettings->setValue("/window x", pos().x());
		configSettings->setValue("/window y", pos().y());
		configSettings->setValue("/window width", size().width());
		configSettings->setValue("/window height", size().height());
>>>>>>> v4.9.227
		configSettings->writeSizes("/split", split->sizes());
		configSettings->endGroup();
	}
}

void ConfigSearchWindow::search(void)
{
	struct symbol **p;
	struct property *prop;
	ConfigItem *lastItem = NULL;

	free(result);
	list->list->clear();
	info->clear();

<<<<<<< HEAD
	result = sym_re_search(editField->text().latin1());
=======
	result = sym_re_search(editField->text().toLatin1());
>>>>>>> v4.9.227
	if (!result)
		return;
	for (p = result; *p; p++) {
		for_all_prompts((*p), prop)
			lastItem = new ConfigItem(list->list, lastItem, prop->menu,
						  menu_is_visible(prop->menu));
	}
}

/*
 * Construct the complete config widget
 */
ConfigMainWindow::ConfigMainWindow(void)
	: searchWindow(0)
{
	QMenuBar* menu;
<<<<<<< HEAD
	bool ok;
	int x, y, width, height;
=======
	bool ok = true;
	QVariant x, y;
	int width, height;
>>>>>>> v4.9.227
	char title[256];

	QDesktopWidget *d = configApp->desktop();
	snprintf(title, sizeof(title), "%s%s",
		rootmenu.prompt->text,
<<<<<<< HEAD
#if QT_VERSION < 0x040000
		" (Qt3)"
#else
		""
#endif
		);
	setCaption(title);

	width = configSettings->readNumEntry("/window width", d->width() - 64);
	height = configSettings->readNumEntry("/window height", d->height() - 64);
	resize(width, height);
	x = configSettings->readNumEntry("/window x", 0, &ok);
	if (ok)
		y = configSettings->readNumEntry("/window y", 0, &ok);
	if (ok)
		move(x, y);
=======
		""
		);
	setWindowTitle(title);

	width = configSettings->value("/window width", d->width() - 64).toInt();
	height = configSettings->value("/window height", d->height() - 64).toInt();
	resize(width, height);
	x = configSettings->value("/window x");
	y = configSettings->value("/window y");
	if ((x.isValid())&&(y.isValid()))
		move(x.toInt(), y.toInt());
>>>>>>> v4.9.227

	split1 = new QSplitter(this);
	split1->setOrientation(Qt::Horizontal);
	setCentralWidget(split1);

	menuView = new ConfigView(split1, "menu");
	menuList = menuView->list;

	split2 = new QSplitter(split1);
	split2->setOrientation(Qt::Vertical);

	// create config tree
	configView = new ConfigView(split2, "config");
	configList = configView->list;

	helpText = new ConfigInfoView(split2, "help");
<<<<<<< HEAD
	helpText->setTextFormat(Qt::RichText);
=======
>>>>>>> v4.9.227

	setTabOrder(configList, helpText);
	configList->setFocus();

	menu = menuBar();
<<<<<<< HEAD
	toolBar = new Q3ToolBar("Tools", this);

	backAction = new Q3Action("Back", QPixmap(xpm_back), _("Back"), 0, this);
	  connect(backAction, SIGNAL(activated()), SLOT(goBack()));
	  backAction->setEnabled(FALSE);
	Q3Action *quitAction = new Q3Action("Quit", _("&Quit"), Qt::CTRL + Qt::Key_Q, this);
	  connect(quitAction, SIGNAL(activated()), SLOT(close()));
	Q3Action *loadAction = new Q3Action("Load", QPixmap(xpm_load), _("&Load"), Qt::CTRL + Qt::Key_L, this);
	  connect(loadAction, SIGNAL(activated()), SLOT(loadConfig()));
	saveAction = new Q3Action("Save", QPixmap(xpm_save), _("&Save"), Qt::CTRL + Qt::Key_S, this);
	  connect(saveAction, SIGNAL(activated()), SLOT(saveConfig()));
	conf_set_changed_callback(conf_changed);
	// Set saveAction's initial state
	conf_changed();
	Q3Action *saveAsAction = new Q3Action("Save As...", _("Save &As..."), 0, this);
	  connect(saveAsAction, SIGNAL(activated()), SLOT(saveConfigAs()));
	Q3Action *searchAction = new Q3Action("Find", _("&Find"), Qt::CTRL + Qt::Key_F, this);
	  connect(searchAction, SIGNAL(activated()), SLOT(searchConfig()));
	Q3Action *singleViewAction = new Q3Action("Single View", QPixmap(xpm_single_view), _("Single View"), 0, this);
	  connect(singleViewAction, SIGNAL(activated()), SLOT(showSingleView()));
	Q3Action *splitViewAction = new Q3Action("Split View", QPixmap(xpm_split_view), _("Split View"), 0, this);
	  connect(splitViewAction, SIGNAL(activated()), SLOT(showSplitView()));
	Q3Action *fullViewAction = new Q3Action("Full View", QPixmap(xpm_tree_view), _("Full View"), 0, this);
	  connect(fullViewAction, SIGNAL(activated()), SLOT(showFullView()));

	Q3Action *showNameAction = new Q3Action(NULL, _("Show Name"), 0, this);
	  showNameAction->setToggleAction(TRUE);
	  connect(showNameAction, SIGNAL(toggled(bool)), configView, SLOT(setShowName(bool)));
	  connect(configView, SIGNAL(showNameChanged(bool)), showNameAction, SLOT(setOn(bool)));
	  showNameAction->setOn(configView->showName());
	Q3Action *showRangeAction = new Q3Action(NULL, _("Show Range"), 0, this);
	  showRangeAction->setToggleAction(TRUE);
	  connect(showRangeAction, SIGNAL(toggled(bool)), configView, SLOT(setShowRange(bool)));
	  connect(configView, SIGNAL(showRangeChanged(bool)), showRangeAction, SLOT(setOn(bool)));
	  showRangeAction->setOn(configList->showRange);
	Q3Action *showDataAction = new Q3Action(NULL, _("Show Data"), 0, this);
	  showDataAction->setToggleAction(TRUE);
	  connect(showDataAction, SIGNAL(toggled(bool)), configView, SLOT(setShowData(bool)));
	  connect(configView, SIGNAL(showDataChanged(bool)), showDataAction, SLOT(setOn(bool)));
	  showDataAction->setOn(configList->showData);

	QActionGroup *optGroup = new QActionGroup(this);
	optGroup->setExclusive(TRUE);
	connect(optGroup, SIGNAL(selected(QAction *)), configView,
		SLOT(setOptionMode(QAction *)));
	connect(optGroup, SIGNAL(selected(QAction *)), menuView,
		SLOT(setOptionMode(QAction *)));

#if QT_VERSION >= 0x040000
	configView->showNormalAction = new QAction(_("Show Normal Options"), optGroup);
	configView->showAllAction = new QAction(_("Show All Options"), optGroup);
	configView->showPromptAction = new QAction(_("Show Prompt Options"), optGroup);
#else
	configView->showNormalAction = new QAction(_("Show Normal Options"), 0, optGroup);
	configView->showAllAction = new QAction(_("Show All Options"), 0, optGroup);
	configView->showPromptAction = new QAction(_("Show Prompt Options"), 0, optGroup);
#endif
	configView->showNormalAction->setToggleAction(TRUE);
	configView->showNormalAction->setOn(configList->optMode == normalOpt);
	configView->showAllAction->setToggleAction(TRUE);
	configView->showAllAction->setOn(configList->optMode == allOpt);
	configView->showPromptAction->setToggleAction(TRUE);
	configView->showPromptAction->setOn(configList->optMode == promptOpt);

	Q3Action *showDebugAction = new Q3Action(NULL, _("Show Debug Info"), 0, this);
	  showDebugAction->setToggleAction(TRUE);
	  connect(showDebugAction, SIGNAL(toggled(bool)), helpText, SLOT(setShowDebug(bool)));
	  connect(helpText, SIGNAL(showDebugChanged(bool)), showDebugAction, SLOT(setOn(bool)));
	  showDebugAction->setOn(helpText->showDebug());

	Q3Action *showIntroAction = new Q3Action(NULL, _("Introduction"), 0, this);
	  connect(showIntroAction, SIGNAL(activated()), SLOT(showIntro()));
	Q3Action *showAboutAction = new Q3Action(NULL, _("About"), 0, this);
	  connect(showAboutAction, SIGNAL(activated()), SLOT(showAbout()));

	// init tool bar
	backAction->addTo(toolBar);
	toolBar->addSeparator();
	loadAction->addTo(toolBar);
	saveAction->addTo(toolBar);
	toolBar->addSeparator();
	singleViewAction->addTo(toolBar);
	splitViewAction->addTo(toolBar);
	fullViewAction->addTo(toolBar);

	// create config menu
	Q3PopupMenu* config = new Q3PopupMenu(this);
	menu->insertItem(_("&File"), config);
	loadAction->addTo(config);
	saveAction->addTo(config);
	saveAsAction->addTo(config);
	config->insertSeparator();
	quitAction->addTo(config);

	// create edit menu
	Q3PopupMenu* editMenu = new Q3PopupMenu(this);
	menu->insertItem(_("&Edit"), editMenu);
	searchAction->addTo(editMenu);

	// create options menu
	Q3PopupMenu* optionMenu = new Q3PopupMenu(this);
	menu->insertItem(_("&Option"), optionMenu);
	showNameAction->addTo(optionMenu);
	showRangeAction->addTo(optionMenu);
	showDataAction->addTo(optionMenu);
	optionMenu->insertSeparator();
	optGroup->addTo(optionMenu);
	optionMenu->insertSeparator();

	// create help menu
	Q3PopupMenu* helpMenu = new Q3PopupMenu(this);
	menu->insertSeparator();
	menu->insertItem(_("&Help"), helpMenu);
	showIntroAction->addTo(helpMenu);
	showAboutAction->addTo(helpMenu);
=======
	toolBar = new QToolBar("Tools", this);
	addToolBar(toolBar);

	backAction = new QAction(QPixmap(xpm_back), _("Back"), this);
	  connect(backAction, SIGNAL(triggered(bool)), SLOT(goBack()));
	  backAction->setEnabled(false);
	QAction *quitAction = new QAction(_("&Quit"), this);
	quitAction->setShortcut(Qt::CTRL + Qt::Key_Q);
	  connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
	QAction *loadAction = new QAction(QPixmap(xpm_load), _("&Load"), this);
	loadAction->setShortcut(Qt::CTRL + Qt::Key_L);
	  connect(loadAction, SIGNAL(triggered(bool)), SLOT(loadConfig()));
	saveAction = new QAction(QPixmap(xpm_save), _("&Save"), this);
	saveAction->setShortcut(Qt::CTRL + Qt::Key_S);
	  connect(saveAction, SIGNAL(triggered(bool)), SLOT(saveConfig()));
	conf_set_changed_callback(conf_changed);
	// Set saveAction's initial state
	conf_changed();
	QAction *saveAsAction = new QAction(_("Save &As..."), this);
	  connect(saveAsAction, SIGNAL(triggered(bool)), SLOT(saveConfigAs()));
	QAction *searchAction = new QAction(_("&Find"), this);
	searchAction->setShortcut(Qt::CTRL + Qt::Key_F);
	  connect(searchAction, SIGNAL(triggered(bool)), SLOT(searchConfig()));
	singleViewAction = new QAction(QPixmap(xpm_single_view), _("Single View"), this);
	singleViewAction->setCheckable(true);
	  connect(singleViewAction, SIGNAL(triggered(bool)), SLOT(showSingleView()));
	splitViewAction = new QAction(QPixmap(xpm_split_view), _("Split View"), this);
	splitViewAction->setCheckable(true);
	  connect(splitViewAction, SIGNAL(triggered(bool)), SLOT(showSplitView()));
	fullViewAction = new QAction(QPixmap(xpm_tree_view), _("Full View"), this);
	fullViewAction->setCheckable(true);
	  connect(fullViewAction, SIGNAL(triggered(bool)), SLOT(showFullView()));

	QAction *showNameAction = new QAction(_("Show Name"), this);
	  showNameAction->setCheckable(true);
	  connect(showNameAction, SIGNAL(toggled(bool)), configView, SLOT(setShowName(bool)));
	  showNameAction->setChecked(configView->showName());
	QAction *showRangeAction = new QAction(_("Show Range"), this);
	  showRangeAction->setCheckable(true);
	  connect(showRangeAction, SIGNAL(toggled(bool)), configView, SLOT(setShowRange(bool)));
	QAction *showDataAction = new QAction(_("Show Data"), this);
	  showDataAction->setCheckable(true);
	  connect(showDataAction, SIGNAL(toggled(bool)), configView, SLOT(setShowData(bool)));

	QActionGroup *optGroup = new QActionGroup(this);
	optGroup->setExclusive(true);
	connect(optGroup, SIGNAL(triggered(QAction*)), configView,
		SLOT(setOptionMode(QAction *)));
	connect(optGroup, SIGNAL(triggered(QAction *)), menuView,
		SLOT(setOptionMode(QAction *)));

	configView->showNormalAction = new QAction(_("Show Normal Options"), optGroup);
	configView->showAllAction = new QAction(_("Show All Options"), optGroup);
	configView->showPromptAction = new QAction(_("Show Prompt Options"), optGroup);
	configView->showNormalAction->setCheckable(true);
	configView->showAllAction->setCheckable(true);
	configView->showPromptAction->setCheckable(true);

	QAction *showDebugAction = new QAction( _("Show Debug Info"), this);
	  showDebugAction->setCheckable(true);
	  connect(showDebugAction, SIGNAL(toggled(bool)), helpText, SLOT(setShowDebug(bool)));
	  showDebugAction->setChecked(helpText->showDebug());

	QAction *showIntroAction = new QAction( _("Introduction"), this);
	  connect(showIntroAction, SIGNAL(triggered(bool)), SLOT(showIntro()));
	QAction *showAboutAction = new QAction( _("About"), this);
	  connect(showAboutAction, SIGNAL(triggered(bool)), SLOT(showAbout()));

	// init tool bar
	toolBar->addAction(backAction);
	toolBar->addSeparator();
	toolBar->addAction(loadAction);
	toolBar->addAction(saveAction);
	toolBar->addSeparator();
	toolBar->addAction(singleViewAction);
	toolBar->addAction(splitViewAction);
	toolBar->addAction(fullViewAction);

	// create config menu
	QMenu* config = menu->addMenu(_("&File"));
	config->addAction(loadAction);
	config->addAction(saveAction);
	config->addAction(saveAsAction);
	config->addSeparator();
	config->addAction(quitAction);

	// create edit menu
	QMenu* editMenu = menu->addMenu(_("&Edit"));
	editMenu->addAction(searchAction);

	// create options menu
	QMenu* optionMenu = menu->addMenu(_("&Option"));
	optionMenu->addAction(showNameAction);
	optionMenu->addAction(showRangeAction);
	optionMenu->addAction(showDataAction);
	optionMenu->addSeparator();
	optionMenu->addActions(optGroup->actions());
	optionMenu->addSeparator();

	// create help menu
	menu->addSeparator();
	QMenu* helpMenu = menu->addMenu(_("&Help"));
	helpMenu->addAction(showIntroAction);
	helpMenu->addAction(showAboutAction);
>>>>>>> v4.9.227

	connect(configList, SIGNAL(menuChanged(struct menu *)),
		helpText, SLOT(setInfo(struct menu *)));
	connect(configList, SIGNAL(menuSelected(struct menu *)),
		SLOT(changeMenu(struct menu *)));
	connect(configList, SIGNAL(parentSelected()),
		SLOT(goBack()));
	connect(menuList, SIGNAL(menuChanged(struct menu *)),
		helpText, SLOT(setInfo(struct menu *)));
	connect(menuList, SIGNAL(menuSelected(struct menu *)),
		SLOT(changeMenu(struct menu *)));

	connect(configList, SIGNAL(gotFocus(struct menu *)),
		helpText, SLOT(setInfo(struct menu *)));
	connect(menuList, SIGNAL(gotFocus(struct menu *)),
		helpText, SLOT(setInfo(struct menu *)));
	connect(menuList, SIGNAL(gotFocus(struct menu *)),
		SLOT(listFocusChanged(void)));
	connect(helpText, SIGNAL(menuSelected(struct menu *)),
		SLOT(setMenuLink(struct menu *)));

<<<<<<< HEAD
	QString listMode = configSettings->readEntry("/listMode", "symbol");
=======
	QString listMode = configSettings->value("/listMode", "symbol").toString();
>>>>>>> v4.9.227
	if (listMode == "single")
		showSingleView();
	else if (listMode == "full")
		showFullView();
	else /*if (listMode == "split")*/
		showSplitView();

	// UI setup done, restore splitter positions
<<<<<<< HEAD
	Q3ValueList<int> sizes = configSettings->readSizes("/split1", &ok);
=======
	QList<int> sizes = configSettings->readSizes("/split1", &ok);
>>>>>>> v4.9.227
	if (ok)
		split1->setSizes(sizes);

	sizes = configSettings->readSizes("/split2", &ok);
	if (ok)
		split2->setSizes(sizes);
}

void ConfigMainWindow::loadConfig(void)
{
<<<<<<< HEAD
	QString s = Q3FileDialog::getOpenFileName(conf_get_configname(), NULL, this);
=======
	QString s = QFileDialog::getOpenFileName(this, "", conf_get_configname());
>>>>>>> v4.9.227
	if (s.isNull())
		return;
	if (conf_read(QFile::encodeName(s)))
		QMessageBox::information(this, "qconf", _("Unable to load configuration!"));
	ConfigView::updateListAll();
}

bool ConfigMainWindow::saveConfig(void)
{
	if (conf_write(NULL)) {
		QMessageBox::information(this, "qconf", _("Unable to save configuration!"));
		return false;
	}
	return true;
}

void ConfigMainWindow::saveConfigAs(void)
{
<<<<<<< HEAD
	QString s = Q3FileDialog::getSaveFileName(conf_get_configname(), NULL, this);
=======
	QString s = QFileDialog::getSaveFileName(this, "", conf_get_configname());
>>>>>>> v4.9.227
	if (s.isNull())
		return;
	saveConfig();
}

void ConfigMainWindow::searchConfig(void)
{
	if (!searchWindow)
		searchWindow = new ConfigSearchWindow(this, "search");
	searchWindow->show();
}

void ConfigMainWindow::changeMenu(struct menu *menu)
{
	configList->setRootMenu(menu);
	if (configList->rootEntry->parent == &rootmenu)
<<<<<<< HEAD
		backAction->setEnabled(FALSE);
	else
		backAction->setEnabled(TRUE);
=======
		backAction->setEnabled(false);
	else
		backAction->setEnabled(true);
>>>>>>> v4.9.227
}

void ConfigMainWindow::setMenuLink(struct menu *menu)
{
	struct menu *parent;
	ConfigList* list = NULL;
	ConfigItem* item;

	if (configList->menuSkip(menu))
		return;

	switch (configList->mode) {
	case singleMode:
		list = configList;
		parent = menu_get_parent_menu(menu);
		if (!parent)
			return;
		list->setRootMenu(parent);
		break;
	case symbolMode:
		if (menu->flags & MENU_ROOT) {
			configList->setRootMenu(menu);
			configList->clearSelection();
			list = menuList;
		} else {
			list = configList;
			parent = menu_get_parent_menu(menu->parent);
			if (!parent)
				return;
			item = menuList->findConfigItem(parent);
			if (item) {
<<<<<<< HEAD
				menuList->setSelected(item, TRUE);
				menuList->ensureItemVisible(item);
=======
				item->setSelected(true);
				menuList->scrollToItem(item);
>>>>>>> v4.9.227
			}
			list->setRootMenu(parent);
		}
		break;
	case fullMode:
		list = configList;
		break;
	default:
		break;
	}

	if (list) {
		item = list->findConfigItem(menu);
		if (item) {
<<<<<<< HEAD
			list->setSelected(item, TRUE);
			list->ensureItemVisible(item);
=======
			item->setSelected(true);
			list->scrollToItem(item);
>>>>>>> v4.9.227
			list->setFocus();
		}
	}
}

void ConfigMainWindow::listFocusChanged(void)
{
	if (menuList->mode == menuMode)
		configList->clearSelection();
}

void ConfigMainWindow::goBack(void)
{
<<<<<<< HEAD
	ConfigItem* item;

	configList->setParentMenu();
	if (configList->rootEntry == &rootmenu)
		backAction->setEnabled(FALSE);
	item = (ConfigItem*)menuList->selectedItem();
	while (item) {
		if (item->menu == configList->rootEntry) {
			menuList->setSelected(item, TRUE);
=======
	ConfigItem* item, *oldSelection;

	configList->setParentMenu();
	if (configList->rootEntry == &rootmenu)
		backAction->setEnabled(false);

	if (menuList->selectedItems().count() == 0)
		return;

	item = (ConfigItem*)menuList->selectedItems().first();
	oldSelection = item;
	while (item) {
		if (item->menu == configList->rootEntry) {
			oldSelection->setSelected(false);
			item->setSelected(true);
>>>>>>> v4.9.227
			break;
		}
		item = (ConfigItem*)item->parent();
	}
}

void ConfigMainWindow::showSingleView(void)
{
<<<<<<< HEAD
=======
	singleViewAction->setEnabled(false);
	singleViewAction->setChecked(true);
	splitViewAction->setEnabled(true);
	splitViewAction->setChecked(false);
	fullViewAction->setEnabled(true);
	fullViewAction->setChecked(false);

>>>>>>> v4.9.227
	menuView->hide();
	menuList->setRootMenu(0);
	configList->mode = singleMode;
	if (configList->rootEntry == &rootmenu)
		configList->updateListAll();
	else
		configList->setRootMenu(&rootmenu);
<<<<<<< HEAD
	configList->setAllOpen(TRUE);
=======
>>>>>>> v4.9.227
	configList->setFocus();
}

void ConfigMainWindow::showSplitView(void)
{
<<<<<<< HEAD
=======
	singleViewAction->setEnabled(true);
	singleViewAction->setChecked(false);
	splitViewAction->setEnabled(false);
	splitViewAction->setChecked(true);
	fullViewAction->setEnabled(true);
	fullViewAction->setChecked(false);

>>>>>>> v4.9.227
	configList->mode = symbolMode;
	if (configList->rootEntry == &rootmenu)
		configList->updateListAll();
	else
		configList->setRootMenu(&rootmenu);
<<<<<<< HEAD
	configList->setAllOpen(TRUE);
	configApp->processEvents();
	menuList->mode = menuMode;
	menuList->setRootMenu(&rootmenu);
	menuList->setAllOpen(TRUE);
=======
	configList->setAllOpen(true);
	configApp->processEvents();
	menuList->mode = menuMode;
	menuList->setRootMenu(&rootmenu);
	menuList->setAllOpen(true);
>>>>>>> v4.9.227
	menuView->show();
	menuList->setFocus();
}

void ConfigMainWindow::showFullView(void)
{
<<<<<<< HEAD
=======
	singleViewAction->setEnabled(true);
	singleViewAction->setChecked(false);
	splitViewAction->setEnabled(true);
	splitViewAction->setChecked(false);
	fullViewAction->setEnabled(false);
	fullViewAction->setChecked(true);

>>>>>>> v4.9.227
	menuView->hide();
	menuList->setRootMenu(0);
	configList->mode = fullMode;
	if (configList->rootEntry == &rootmenu)
		configList->updateListAll();
	else
		configList->setRootMenu(&rootmenu);
<<<<<<< HEAD
	configList->setAllOpen(FALSE);
=======
>>>>>>> v4.9.227
	configList->setFocus();
}

/*
 * ask for saving configuration before quitting
 * TODO ask only when something changed
 */
void ConfigMainWindow::closeEvent(QCloseEvent* e)
{
	if (!conf_get_changed()) {
		e->accept();
		return;
	}
	QMessageBox mb("qconf", _("Save configuration?"), QMessageBox::Warning,
			QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape);
	mb.setButtonText(QMessageBox::Yes, _("&Save Changes"));
	mb.setButtonText(QMessageBox::No, _("&Discard Changes"));
	mb.setButtonText(QMessageBox::Cancel, _("Cancel Exit"));
	switch (mb.exec()) {
	case QMessageBox::Yes:
		if (saveConfig())
			e->accept();
		else
			e->ignore();
		break;
	case QMessageBox::No:
		e->accept();
		break;
	case QMessageBox::Cancel:
		e->ignore();
		break;
	}
}

void ConfigMainWindow::showIntro(void)
{
	static const QString str = _("Welcome to the qconf graphical configuration tool.\n\n"
		"For each option, a blank box indicates the feature is disabled, a check\n"
		"indicates it is enabled, and a dot indicates that it is to be compiled\n"
		"as a module.  Clicking on the box will cycle through the three states.\n\n"
		"If you do not see an option (e.g., a device driver) that you believe\n"
		"should be present, try turning on Show All Options under the Options menu.\n"
		"Although there is no cross reference yet to help you figure out what other\n"
		"options must be enabled to support the option you are interested in, you can\n"
		"still view the help of a grayed-out option.\n\n"
		"Toggling Show Debug Info under the Options menu will show the dependencies,\n"
		"which you can then match by examining other options.\n\n");

	QMessageBox::information(this, "qconf", str);
}

void ConfigMainWindow::showAbout(void)
{
<<<<<<< HEAD
	static const QString str = _("qconf is Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>.\n\n"
=======
	static const QString str = _("qconf is Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>.\n"
		"Copyright (C) 2015 Boris Barbulovski <bbarbulovski@gmail.com>.\n\n"
>>>>>>> v4.9.227
		"Bug reports and feature request can also be entered at http://bugzilla.kernel.org/\n");

	QMessageBox::information(this, "qconf", str);
}

void ConfigMainWindow::saveSettings(void)
{
<<<<<<< HEAD
	configSettings->writeEntry("/window x", pos().x());
	configSettings->writeEntry("/window y", pos().y());
	configSettings->writeEntry("/window width", size().width());
	configSettings->writeEntry("/window height", size().height());
=======
	configSettings->setValue("/window x", pos().x());
	configSettings->setValue("/window y", pos().y());
	configSettings->setValue("/window width", size().width());
	configSettings->setValue("/window height", size().height());
>>>>>>> v4.9.227

	QString entry;
	switch(configList->mode) {
	case singleMode :
		entry = "single";
		break;

	case symbolMode :
		entry = "split";
		break;

	case fullMode :
		entry = "full";
		break;

	default:
		break;
	}
<<<<<<< HEAD
	configSettings->writeEntry("/listMode", entry);
=======
	configSettings->setValue("/listMode", entry);
>>>>>>> v4.9.227

	configSettings->writeSizes("/split1", split1->sizes());
	configSettings->writeSizes("/split2", split2->sizes());
}

void ConfigMainWindow::conf_changed(void)
{
	if (saveAction)
		saveAction->setEnabled(conf_get_changed());
}

void fixup_rootmenu(struct menu *menu)
{
	struct menu *child;
	static int menu_cnt = 0;

	menu->flags |= MENU_ROOT;
	for (child = menu->list; child; child = child->next) {
		if (child->prompt && child->prompt->type == P_MENU) {
			menu_cnt++;
			fixup_rootmenu(child);
			menu_cnt--;
		} else if (!menu_cnt)
			fixup_rootmenu(child);
	}
}

static const char *progname;

static void usage(void)
{
<<<<<<< HEAD
	printf(_("%s [-s] <config>\n"), progname);
=======
	printf(_("%s [-s] <config>\n").toLatin1().constData(), progname);
>>>>>>> v4.9.227
	exit(0);
}

int main(int ac, char** av)
{
	ConfigMainWindow* v;
	const char *name;

	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	progname = av[0];
	configApp = new QApplication(ac, av);
	if (ac > 1 && av[1][0] == '-') {
		switch (av[1][1]) {
		case 's':
			conf_set_message_callback(NULL);
			break;
		case 'h':
		case '?':
			usage();
		}
		name = av[2];
	} else
		name = av[1];
	if (!name)
		usage();

	conf_parse(name);
	fixup_rootmenu(&rootmenu);
	conf_read(NULL);
	//zconfdump(stdout);

	configSettings = new ConfigSettings();
	configSettings->beginGroup("/kconfig/qconf");
	v = new ConfigMainWindow();

	//zconfdump(stdout);
<<<<<<< HEAD
	configApp->setMainWidget(v);
=======
>>>>>>> v4.9.227
	configApp->connect(configApp, SIGNAL(lastWindowClosed()), SLOT(quit()));
	configApp->connect(configApp, SIGNAL(aboutToQuit()), v, SLOT(saveSettings()));
	v->show();
	configApp->exec();

	configSettings->endGroup();
	delete configSettings;
<<<<<<< HEAD
=======
	delete v;
	delete configApp;
>>>>>>> v4.9.227

	return 0;
}
