// Sample extension code, makes clicking on the panel show a message
const Overview = imports.ui.overview;
const St = imports.gi.St;
const Mainloop = imports.mainloop;
const IconGrid = imports.ui.iconGrid;
const Main = imports.ui.main;
const Shell = imports.gi.Shell;
const Clutter = imports.gi.Clutter;
const Store = imports.gi.GnomeAppStore;

function MyAppIcon(app, params) {
    this._init(app, params);
}

MyAppIcon.prototype = {
    __proto__:  IconGrid.BaseIcon.prototype,

    _init : function(store, id, params) {
        let label = store.get_name_from_id (id);

        IconGrid.BaseIcon.prototype._init.call(this,
                                               label,
                                               params);
    },
};

function MyAppWellIcon(app, iconParams) {
    this._init(app, iconParams);
}

MyAppWellIcon.prototype = {
    _init : function(store, id, iconParams) {
        this.actor = new St.Button({ style_class: 'myapp-well-app',
                                     reactive: true,
                                     button_mask: St.ButtonMask.ONE | St.ButtonMask.TWO,
                                     can_focus: true,
                                     x_fill: true,
                                     y_fill: true });
        this.actor._delegate = this;

        this.icon = new MyAppIcon(store, id, iconParams);
        this.actor.set_child(this.icon.actor);

        this.actor.label_actor = this.icon.label;
    }
}

function _showHello() {
    let text = new St.Label({ style_class: 'helloworld-label', text: "David said: Harmony world!" });
    let monitor = global.get_primary_monitor();
    global.stage.add_actor(text);
    text.set_position(Math.floor (monitor.width / 2 - text.width / 2), Math.floor(monitor.height / 2 - text.height / 2));
    Mainloop.timeout_add(3000, function () { text.destroy(); });
}

// Put your extension initialization code here
function main() {
//-------------------app store -----------
   let box = new St.BoxLayout({ vertical: true });
   let stage = new Store.AppStage ();

   box.add(stage);

   let store = new Store.AppStore ();
   let app_act;
   let i, item, name, text1;
   let is_default;
   let counts;
   counts = store.get_counts ();

   counts = 100;

   for (i=0; i< counts; i++) {
	item = store.get_nth_app (i);
	app_act = item.get_icon ();
        is_default = item.is_default_icon ();
	if (!is_default) {
		stage.add_actor (app_act);
	}
    }

   Main.overview.viewSelector.addViewTab('gnome-app-store', _("GNOME App Store"), box);
}
