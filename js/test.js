const Clutter = imports.gi.Clutter;
const Widgets = imports.gi.GnomeAppStoreWidgets;
const Common = imports.gi.GnomeAppStoreCommon;
const Rest = imports.gi.Rest;

Clutter.init(null, 0);

let stage = new Clutter.Stage();

stage.set_size (1000, 700);
let texture = new Clutter.Texture({ filename: 'test.jpg',
                                    reactive: true });

function dl_cb() {
};

function dl_button_press(){
	log ("pres\n");
	let store = new Store.AppStore ();
	let counts = store.get_counts ();
	let i, item, app_act, is_default;
	for (i=0; i< counts; i++) {
		item = store.get_nth_app (i);
 //    		app_act = item.get_icon ();
		is_default = item.is_default_icon ();
//		stage.add_actor (app_act);
	}

	log (counts);
};

texture.connect('button-press-event',
                function(o, event) {
                    log('Clicked!');
                    return true;
                });

let color = new Clutter.Color();
color.from_string('White');

let button = new Widgets.AppButton();
button.set_text ("DL button");
button.connect('button-press-event',
		dl_button_press);

let config = Common.app_get_spin_dir();
log (config);

let url = "http://localhost:3000";
let prop = {"url_format" : url, "binding_required" : false};
let proxy = new Rest.Proxy(prop);
let call = new Rest.ProxyCall();
call.set_proxy (proxy);
call.set_function ("/content/data");
log (call.get_function ());
call.cancel ();

let script = new Clutter.Script ();

script.load_from_file ("../ui/app-frame.json");

let frame = script.get_object ("frame");
stage.add_actor (frame);
stage.color = color;

stage.show();

Clutter.main();
