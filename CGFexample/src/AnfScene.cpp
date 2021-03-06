#include "AnfScene.h"
#include "CGFaxis.h"
#include "CGFapplication.h"
#include <math.h>
#include <iostream>
#include <typeinfo>

float pi = acos(-1.0);
float deg2rad = pi / 180.0;

#define BOARD_HEIGHT 6.0
#define BOARD_WIDTH 6.4

AnfScene::AnfScene(char* filename) {
	t = 0;
	t0 = 0;
	game = new Game();
	cout << game->to_string() << endl;
//	Textures::Texture deft;
//	deft.file="PoleText.jpg";
	defapp = new CGFappearance();
	float f[4];
	f[0] = 0.5;
	f[1] = 0.5;
	f[2] = 0.5;
	f[3] = 1;
	defapp->setAmbient(f);
	defapp->setSpecular(f);
	defapp->setDiffuse(f);
	defapp->setShininess(2);
	defapp->setTexture("poleText.jpg");
	velocity = 2;
	listNumber = 1;
	TiXmlElement* globalsElement;
	TiXmlElement* camerasElement;
	TiXmlElement* lightsElement;
	TiXmlElement* textsElement;
	TiXmlElement* appearsElement;
	TiXmlElement* graphElement;
	TiXmlElement* animationsElement;

	TiXmlDocument *doc = new TiXmlDocument(filename);

	if (!doc->LoadFile()) {
		printf("Could not load file '%s'. Error='%s'. Exiting.\n", filename, doc->ErrorDesc());
		exit(1);
	}

	TiXmlElement* anfElement = doc->FirstChildElement("anf");

	if (anfElement == NULL) {
		printf("Main anf block element not found! Exiting!\n");
		exit(1);
	}

	globalsElement = anfElement->FirstChildElement("globals");
	camerasElement = anfElement->FirstChildElement("cameras");
	lightsElement = anfElement->FirstChildElement("lights");
	textsElement = anfElement->FirstChildElement("textures");
	appearsElement = anfElement->FirstChildElement("appearances");
	animationsElement = anfElement->FirstChildElement("animations");
	graphElement = anfElement->FirstChildElement("graph");

	printf("Starting globals\n");
	if (globalsElement == NULL)
		printf("Globals block not found!\n");
	else {
		TiXmlElement* drawingElement = globalsElement->FirstChildElement("drawing");
		char *shading;
		string mode;
		float background[4];
		mode = (char*) drawingElement->Attribute("mode");
		shading = (char*) drawingElement->Attribute("shading");
		sscanf((char*) drawingElement->Attribute("background"), "%f %f %f %f", &background[0], &background[1], &background[2], &background[3]);

		if (mode == "fill")
			globals.drawing.mode = 0;
		else if (mode == "line")
			globals.drawing.mode = 1;
		else
			globals.drawing.mode = 2;
		globals.drawing.shadding = shading;
		for (int i = 0; i < 4; ++i)
			globals.drawing.background.push_back(background[i]);

		TiXmlElement* cullingElement = globalsElement->FirstChildElement("culling");
		char *face, *order;
		face = (char*) cullingElement->Attribute("face");
		order = (char*) cullingElement->Attribute("order");

		globals.culling.face = face;
		globals.culling.order = order;

		TiXmlElement* lightingElement = globalsElement->FirstChildElement("lighting");
		bool doublesided, local, enabled;
		char *sambient;
		float ambient[4];
		lightingElement->QueryBoolAttribute("doublesided", &doublesided);
		lightingElement->QueryBoolAttribute("local", &local);
		lightingElement->QueryBoolAttribute("enabled", &enabled);
		sambient = (char*) lightingElement->Attribute("ambient");
		sscanf(sambient, "%f %f %f %f", &ambient[0], &ambient[1], &ambient[2], &ambient[3]);
		globals.lighting.doublesided = doublesided;
		globals.lighting.local = local;
		globals.lighting.enabled = enabled;
		for (int i = 0; i < 4; ++i) {
			globals.lighting.ambient.push_back(ambient[i]);
		}

	}

	printf("Read globals\nStarting Cameras\n");

	if (camerasElement == NULL)
		printf("Cameras block not found!\n");
	else {
		string initial;
		initial = (char*) camerasElement->Attribute("initial");

		for (TiXmlElement* p = camerasElement->FirstChildElement("perspective"); p != NULL; p = p->NextSiblingElement("perspective")) {
			string id;
			float near, far, angle;
			float pos[3], target[3];
			id = (char*) p->Attribute("id");
			p->QueryFloatAttribute("near", &near);
			p->QueryFloatAttribute("far", &far);
			p->QueryFloatAttribute("angle", &angle);
			sscanf(p->Attribute("pos"), "%f %f %f", &pos[0], &pos[1], &pos[2]);
			sscanf(p->Attribute("target"), "%f %f %f", &target[0], &target[1], &target[2]);

			CPerspective* cp = new CPerspective();

			cp->angle = angle;
			cp->far = far;
			cp->near = near;

			cp->pos[0] = pos[0];
			cp->pos[1] = pos[1];
			cp->pos[2] = pos[2];

			cp->target[0] = target[0];
			cp->target[1] = target[1];
			cp->target[2] = target[2];

			cameras[id] = cp;
			camera_id.push_back(id);

			if (id == initial)
				this->initial = camera_id.size() - 1;
		}
		for (TiXmlElement* o = camerasElement->FirstChildElement("ortho"); o != NULL; o = o->NextSiblingElement("ortho")) {
			string id;
			char *direction;
			float near, far, left, right, top, bottom;
			id = (char*) o->Attribute("id");
			direction = (char*) o->Attribute("direction");
			o->QueryFloatAttribute("near", &near);
			o->QueryFloatAttribute("far", &far);
			o->QueryFloatAttribute("left", &left);
			o->QueryFloatAttribute("right", &right);
			o->QueryFloatAttribute("top", &top);
			o->QueryFloatAttribute("bottom", &bottom);

			COrtho* co = new COrtho();

			co->near = near;
			co->far = far;
			co->left = left;
			co->right = right;
			co->top = top;
			co->bottom = bottom;
			co->direction = direction;

			cameras[id] = co;
			camera_id.push_back(id);

			if (id == initial)
				this->initial = camera_id.size() - 1;
		}
	}

	printf("Read Cameras\nStarting lights\n");

	if (lightsElement == NULL)
		printf("Lights block not found!\n");
	else {
		int idl[8];
		idl[0] = GL_LIGHT0;
		idl[1] = GL_LIGHT1;
		idl[2] = GL_LIGHT2;
		idl[3] = GL_LIGHT3;
		idl[4] = GL_LIGHT4;
		idl[5] = GL_LIGHT5;
		idl[6] = GL_LIGHT6;
		idl[7] = GL_LIGHT7;
		int idlaux = 0;

		for (TiXmlElement* l = lightsElement->FirstChildElement("light"); l != NULL; l = l->NextSiblingElement("light")) {

			Light light;
			char *type;
			type = (char*) l->Attribute("type");
			if (type == 0)
				continue;
			float pos[4];
			if (sscanf(l->Attribute("pos"), "%f %f %f", &pos[0], &pos[1], &pos[2]) != 3)
				continue;
			pos[3] = 1;
			char *id = (char*) l->Attribute("id");
			if (id == 0)
				continue;

			bool enabled, marker;
			if (l->QueryBoolAttribute("enabled", &enabled) != TIXML_SUCCESS)
				continue;
			if (l->QueryBoolAttribute("marker", &marker) != TIXML_SUCCESS)
				continue;

			if (strcmp(type, "spot") == 0) {
				float target[3], angle, exponent;

				if (l->QueryFloatAttribute("angle", &angle) != TIXML_SUCCESS)
					continue;
				if (l->QueryFloatAttribute("exponent", &exponent) != TIXML_SUCCESS)
					continue;

				if (sscanf(l->Attribute("target"), "%f %f %f", &target[0], &target[1], &target[2]) != 3)
					continue;
				float unit = sqrt(target[0] * target[0] + target[1] * target[1] + target[2] * target[2]);
				for (int i = 0; i < 3; i++) {
					target[i] = target[i] / unit;
					cout << "target " << target[i] << endl;
				}
				glLightf(idl[idlaux], GL_SPOT_CUTOFF, angle);
				glLightf(idl[idlaux], GL_SPOT_EXPONENT, exponent);
				glLightfv(idl[idlaux], GL_SPOT_DIRECTION, target);

			}
			light.cgfl = new CGFlight(idl[idlaux], pos);
			++idlaux;

			for (TiXmlElement* c = l->FirstChildElement("component"); c != NULL; c = c->NextSiblingElement("component")) {
				char* type = (char*) c->Attribute("type");
				float value[4];
				for (int i = 0; i < 4; ++i)
					value[i] = 0;
				sscanf(c->Attribute("value"), "%f %f %f %f", &value[0], &value[1], &value[2], &value[3]);
				if (strcmp(type, "ambient") == 0) {
					light.cgfl->setAmbient(value);
				} else if (strcmp(type, "diffuse") == 0) {
					light.cgfl->setDiffuse(value);
				} else if (strcmp(type, "specular") == 0) {
					light.cgfl->setSpecular(value);
				}
			}

			light.marker = marker;

			if (enabled) {
				light.enabled = true;
				light.cgfl->enable();
			} else {
				light.enabled = false;
				light.cgfl->disable();
			}
			light.cgfl->update();
			lights.push_back(light);
			light_id.push_back(id);
		}
	}

	printf("Read lights\nStarting textures\n");

	if (textsElement == NULL)
		printf("Textures block not found!\n");
	else {
		for (TiXmlElement* t = textsElement->FirstChildElement("texture"); t != NULL; t = t->NextSiblingElement("texture")) {
			char *id, *file;
			float textlength_s, textlength_t;
			id = (char*) t->Attribute("id");
			if (id == 0)
				continue;
			file = (char*) t->Attribute("file");
			if (file == 0)
				continue;
			if (t->QueryFloatAttribute("texlength_s", &textlength_s) != TIXML_SUCCESS)
				continue;
			if (t->QueryFloatAttribute("texlength_t", &textlength_t) != TIXML_SUCCESS)
				continue;
			Textures::Texture text;
			text.file = file;
			text.texlength_s = textlength_s;
			text.texlength_t = textlength_t;
			textures.texture[id] = text;
		}
	}

	printf("Read textures\nStarting appearences\n");

	if (appearsElement == NULL)
		printf("Appearences block not found!\n");
	else {
		for (TiXmlElement *a = appearsElement->FirstChildElement("appearance"); a != NULL; a = a->NextSiblingElement("appearance")) {
			char *id, *textureref;
			float shininess;
			if (a->QueryFloatAttribute("shininess", &shininess) != TIXML_SUCCESS)
				continue;
			id = (char*) a->Attribute("id");
			if (id == 0)
				continue;
			textureref = (char*) a->Attribute("textureref");
			float ambient[4], diffuse[4], specular[4];
			for (TiXmlElement* c = a->FirstChildElement("component"); c != NULL; c = c->NextSiblingElement("component")) {
				char* type = (char*) c->Attribute("type");
				float value[4];
				for (int i = 0; i < 4; ++i)
					value[i] = 0;
				sscanf(c->Attribute("value"), "%f %f %f %f", &value[0], &value[1], &value[2], &value[3]);
				if (strcmp(type, "ambient") == 0) {
					for (int i = 0; i < 4; i++)
						ambient[i] = value[i];
				} else if (strcmp(type, "diffuse") == 0)
					for (int i = 0; i < 4; i++)
						diffuse[i] = value[i];
				else if (strcmp(type, "specular") == 0)
					for (int i = 0; i < 4; i++)
						specular[i] = value[i];
			}
			CGFappearance *ap = new CGFappearance(ambient, diffuse, specular, shininess);
			Appearence app;
			app.appearence = ap;
			if (textureref != NULL && textures.texture[textureref].file != "") {
				app.textureref = textureref;
				cout << "try" << endl;
				ap->setTexture(textures.texture[textureref].file);
				cout << "pass" << endl;
			}
			appearances[id] = app;
		}
	}

	printf("Read appearences\nStarting animations\n");
	if (animationsElement == NULL)
		printf("Animations block not found!\n");
	else {
		for (TiXmlElement *a = animationsElement->FirstChildElement("animation"); a != NULL; a = a->NextSiblingElement("animation")) {
			char* id = (char*) a->Attribute("id");
			char* type = (char*) a->Attribute("type");
			float time;
			a->QueryFloatAttribute("span", &time);
			if (strcmp(type, "linear") == 0) {
				vector<vector<float> > points;
				for (TiXmlElement *cp = a->FirstChildElement("controlpoint"); cp != NULL; cp = cp->NextSiblingElement("controlpoint")) {
					vector<float> p(3);
					cp->QueryFloatAttribute("xx", &p[0]);
					cp->QueryFloatAttribute("yy", &p[1]);
					cp->QueryFloatAttribute("zz", &p[2]);
					points.push_back(p);
				}

				vector<float> dist;
				vector<vector<float> > dirs;
				float tdist = 0;
				for (unsigned int i = 0; i < points.size() - 1; ++i) {
					dist.push_back(
							sqrt(
									pow(points[i + 1][0] - points[i][0], 2) + pow(points[i + 1][1] - points[i][1], 2)
											+ pow(points[i + 1][2] - points[i][2], 2)));
					tdist += dist[i];

					vector<float> dir;
					dir.push_back((points[i + 1][0] - points[i][0]) / dist[i]);
					dir.push_back((points[i + 1][1] - points[i][1]) / dist[i]);
					dir.push_back((points[i + 1][2] - points[i][2]) / dist[i]);
					cout << "dir " << dir[0] << dir[1] << dir[2] << endl;
					dirs.push_back(dir);
				}

				vector<float> times;
				float d = 0;
				for (unsigned int i = 0; i < dist.size(); ++i) {
					d += dist[i];
					times.push_back((d * time) / tdist);
					cout << "time " << times[i] << endl;
				}
				LinearAnimation* lan = new LinearAnimation();
				lan->currentDir = 0;
				lan->dir = dirs;
				lan->pos = points;
				lan->start = 0;
				lan->time = times;
				lan->v = tdist / time;
				anim[id] = lan;
			} else {
				float sang, da, r;
				a->QueryFloatAttribute("startang", &sang);
				a->QueryFloatAttribute("rotang", &da);
				a->QueryFloatAttribute("radius", &r);

				vector<float> c(3);
				sscanf(a->Attribute("center"), "%f %f %f", &c[0], &c[1], &c[2]);

				CircularAnimation* ca = new CircularAnimation;
				cout << da << ":" << time << endl;
				ca->sang = sang;
				ca->start = 0;
				ca->time = time;
				ca->vang = da / time;
				ca->center = c;
				ca->r = r;
				anim[id] = ca;
			}
		}
	}

	if (graphElement == NULL)
		printf("Graph block not found!\n");
	else {
		char *rootid = (char*) graphElement->Attribute("rootid");
		if (rootid == 0) {
			cout << "No root id!\n";
			return;
		}
		graph.rootid = rootid;

		for (TiXmlElement* n = graphElement->FirstChildElement("node"); n != NULL; n = n->NextSiblingElement("node")) {

			Graph::Node node;
			node.currentAnim = 0;
			char* id = (char*) n->Attribute("id");
			if (id == 0)
				continue;

			for (TiXmlElement* an = n->FirstChildElement("animationref"); an != NULL; an = an->NextSiblingElement("animationref")) {
				node.anim.push_back(anim[an->Attribute("id")]);
			}

			bool displaylist = false;
			n->QueryBoolAttribute("displaylist", &displaylist);
			node.displaylist = displaylist;
			glLoadIdentity();
			TiXmlElement* transformElement = n->FirstChildElement("transforms");
			if (transformElement != NULL) {
				for (TiXmlElement* t = transformElement->FirstChildElement("transform"); t != NULL; t = t->NextSiblingElement("transform")) {

					char* type;
					type = (char*) t->Attribute("type");

					if (strcmp(type, "translate") == 0) {
						float to[3];
						for (int i = 0; i < 3; ++i)
							to[i] = 0;
						sscanf(t->Attribute("to"), "%f %f %f", &to[0], &to[1], &to[2]);
						glTranslatef(to[0], to[1], to[2]);
					} else if (strcmp(type, "rotate") == 0) {
						string axis;
						float angle;
						axis = (char*) t->Attribute("axis");
						if (t->QueryFloatAttribute("angle", &angle) != TIXML_SUCCESS)
							continue;
						glRotatef(angle, axis == "x", axis == "y", axis == "z");
					} else {
						float factor[3];
						for (int i = 0; i < 3; ++i)
							factor[i] = 0;
						sscanf(t->Attribute("factor"), "%f %f %f", &factor[0], &factor[1], &factor[2]);
						glScalef(factor[0], factor[1], factor[2]);
					}
				}
			}
			glGetFloatv(GL_MODELVIEW_MATRIX, node.matrix);

			char* appearancerefId;
			TiXmlElement* apElement = n->FirstChildElement("appearanceref");
			if (apElement != NULL) {
				appearancerefId = (char*) apElement->Attribute("id");
				node.appearencerefID = appearancerefId;
			}

			TiXmlElement* primitivesElement = n->FirstChildElement("primitives");
			if (primitivesElement != NULL) {
				for (TiXmlElement* p = primitivesElement->FirstChildElement("rectangle"); p != NULL; p = p->NextSiblingElement("rectangle")) {
					float xy1[2], xy2[2];
					for (int i = 0; i < 2; ++i)
						xy1[i] = 0;
					for (int i = 0; i < 2; ++i)
						xy2[i] = 0;
					sscanf(p->Attribute("xy1"), "%f %f", &xy1[0], &xy1[1]);
					sscanf(p->Attribute("xy2"), "%f %f", &xy2[0], &xy2[1]);
					Graph::Node::Rectangle rec;
					for (int i = 0; i < 2; ++i)
						rec.xy1.push_back(xy1[i]);
					for (int i = 0; i < 2; ++i)
						rec.xy2.push_back(xy2[i]);
					node.rectangle.push_back(rec);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("triangle"); p != NULL; p = p->NextSiblingElement("triangle")) {
					float xyz1[3], xyz2[3], xyz3[3];

					for (int i = 0; i < 3; ++i)
						xyz1[i] = 0;
					for (int i = 0; i < 3; ++i)
						xyz2[i] = 0;
					for (int i = 0; i < 3; ++i)
						xyz3[i] = 0;
					sscanf(p->Attribute("xyz1"), "%f %f %f", &xyz1[0], &xyz1[1], &xyz1[2]);
					sscanf(p->Attribute("xyz2"), "%f %f %f", &xyz2[0], &xyz2[1], &xyz2[2]);
					sscanf(p->Attribute("xyz3"), "%f %f %f", &xyz3[0], &xyz3[1], &xyz3[2]);
					Graph::Node::Triangle tr;
					for (int i = 0; i < 3; ++i)
						tr.xyz1.push_back(xyz1[i]);
					for (int i = 0; i < 3; ++i)
						tr.xyz2.push_back(xyz2[i]);
					for (int i = 0; i < 3; ++i)
						tr.xyz3.push_back(xyz3[i]);
					node.triangle.push_back(tr);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("cylinder"); p != NULL; p = p->NextSiblingElement("cylinder")) {
					float base, top, height;
					int slices, stacks;
					if (p->QueryFloatAttribute("base", &base) != TIXML_SUCCESS)
						continue;
					if (p->QueryFloatAttribute("top", &top) != TIXML_SUCCESS)
						continue;
					if (p->QueryFloatAttribute("height", &height) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("slices", &slices) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("stacks", &stacks) != TIXML_SUCCESS)
						continue;
					Graph::Node::Cylinder c;
					c.base = base;
					c.height = height;
					c.slices = slices;
					c.stacks = stacks;
					c.top = top;
					node.cylinder.push_back(c);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("sphere"); p != NULL; p = p->NextSiblingElement("sphere")) {
					float radius;
					int slices, stacks;

					if (p->QueryFloatAttribute("radius", &radius) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("slices", &slices) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("stacks", &stacks) != TIXML_SUCCESS)
						continue;
					Graph::Node::Sphere s;
					s.radius = radius;
					s.slices = slices;
					s.stacks = stacks;
					node.sphere.push_back(s);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("torus"); p != NULL; p = p->NextSiblingElement("torus")) {
					float inner, outer;
					int slices, loops;
					if (p->QueryFloatAttribute("inner", &inner) != TIXML_SUCCESS)
						continue;
					if (p->QueryFloatAttribute("outer", &outer) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("slices", &slices) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("loops", &loops) != TIXML_SUCCESS)
						continue;
					Graph::Node::Torus t;
					t.inner = inner;
					t.outer = outer;
					t.slices = slices;
					t.loops = loops;
					node.torus.push_back(t);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("plane"); p != NULL; p = p->NextSiblingElement("plane")) {
					float parts;
					if (p->QueryFloatAttribute("parts", &parts) != TIXML_SUCCESS)
						continue;
					Graph::Node::Plane plane(parts);
					node.plane.push_back(plane);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("patch"); p != NULL; p = p->NextSiblingElement("patch")) {

					int order, partsU, partsV;
					string compute;
					vector<GLfloat> controlpoints;
					if (p->QueryIntAttribute("order", &order) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("partsU", &partsU) != TIXML_SUCCESS)
						continue;
					if (p->QueryIntAttribute("partsV", &partsV) != TIXML_SUCCESS)
						continue;
					compute = p->Attribute("compute");

					for (TiXmlElement* cp = p->FirstChildElement(); cp != NULL; cp = cp->NextSiblingElement()) {
						float x, y, z;
						cp->QueryFloatAttribute("x", &x);
						cp->QueryFloatAttribute("y", &y);
						cp->QueryFloatAttribute("z", &z);
						controlpoints.push_back(x);
						controlpoints.push_back(y);
						controlpoints.push_back(z);
					}
					Graph::Node::Patch patch(order, partsU, partsV, compute, controlpoints);
					node.patch.push_back(patch);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("flag"); p != NULL; p = p->NextSiblingElement("flag")) {

					string text;
					text = p->Attribute("texture");
					Graph::Node::Flag flag;

					flag.plane = new Graph::Node::Plane(100);

					flag.text = text;
					flag.shader = new FlagShadder(text);
					node.flag.push_back(flag);
					shaders.push_back(flag.shader);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("vehicle"); p != NULL; p = p->NextSiblingElement("vehicle")) {
					Graph::Node::Seagull s;
					node.seagull.push_back(s);
				}
			}

			TiXmlElement* descendantsElement = n->FirstChildElement("descendants");
			if (descendantsElement != NULL) {
				for (TiXmlElement* d = descendantsElement->FirstChildElement("noderef"); d != NULL; d = d->NextSiblingElement("noderef")) {
					char* idref = (char*) d->Attribute("id");
					Graph::Node* p;
					p = &graph.nodes[idref];
					node.descendant.push_back(p);
				}
			}
			node.appearencerefID = appearancerefId;
			graph.nodes[id] = node;

		}
	}
	printf("Read graph\nRead complete!\n");
}

void AnfScene::init() {
	socketConnect();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	Declares and enables a light
	if (globals.culling.face == "none")
		glCullFace(GL_NONE);
	else if (globals.culling.face == "back")
		glCullFace(GL_BACK);
	else if (globals.culling.face == "front")
		glCullFace(GL_FRONT);
	else if (globals.culling.face == "both")
		glCullFace(GL_FRONT_AND_BACK);
	if (globals.culling.order == "ccw")
		glFrontFace(GL_CCW);
	else if (globals.culling.order == "cw")
		glFrontFace(GL_CW);

	if (globals.lighting.enabled)
		glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	if (globals.lighting.doublesided)
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if (globals.lighting.local)
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	// Defines a default normal

	float ambientLight[4] = { 0, 0, 0, 0 };
	for (unsigned int i = 0; i < globals.lighting.ambient.size(); i++) {
		ambientLight[i] = 1;
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	if (globals.drawing.shadding == "gouraud")
		glShadeModel(GL_SMOOTH);
	else if (globals.drawing.shadding == "flat")
		glShadeModel(GL_FLAT);
	glClearColor(0.5,0.5,0.5,1);
	glNormal3f(0, 0, 1);

	setUpdatePeriod(5);

	createList(&graph.nodes[graph.rootid], "");
}

void drawPatch(string compute, int order, int partsU, int partsV, vector<GLfloat> controlpoints, vector<GLfloat> textpoints) {
	glFrontFace(GL_CW);
	glEnable(GL_AUTO_NORMAL);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, order + 1, 0.0, 1.0, 3 * (order + 1), order + 1, &controlpoints[0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, 2, order + 1, 0.0, 1.0, 2 * (order + 1), order + 1, &textpoints[0]);

	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	glMapGrid2f(partsU, 0.0, 1.0, partsV, 0.0, 1.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	if (compute == "line")
		glEvalMesh2(GL_LINE, 0, partsU, 0, partsV);
	else if (compute == "point")
		glEvalMesh2(GL_LINE, 0, partsU, 0, partsV);
	else
		glEvalMesh2(GL_FILL, 0, partsU, 0, partsV);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glFrontFace(GL_CCW);
}
void drawRectangle(float x1, float y1, float x2, float y2) {
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex2f(x1, y1);
	glTexCoord2f(1, 0);
	glVertex2f(x2, y1);
	glTexCoord2f(1, 1);
	glVertex2f(x2, y2);
	glTexCoord2f(0, 1);
	glVertex2f(x1, y2);
	glEnd();
}
void drawTriangle(vector<float> v1, vector<float> v2, vector<float> v3) {
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0, 0);
	glVertex3f(v1[0], v1[1], v1[2]);
	glTexCoord2f(1, 0);
	glVertex3f(v2[0], v2[1], v2[2]);
	glTexCoord2f(0.5, 1);
	glVertex3f(v3[0], v3[1], v3[2]);
	glEnd();
}
void drawCylinder(float base, float top, float height, int slices, int stacks) {
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	GLUquadric *botD = gluNewQuadric();
	gluQuadricTexture(botD, GL_TRUE);
	gluDisk(botD, 0, base, slices, 1);
	1;
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, height);
	GLUquadric *topD = gluNewQuadric();
	gluQuadricTexture(topD, GL_TRUE);
	gluDisk(topD, 0, top, slices, 1);
	glPopMatrix();
	GLUquadric *cyl = gluNewQuadric();
	gluQuadricTexture(cyl, GL_TRUE);
	gluCylinder(cyl, base, top, height, slices, stacks);

}
void drawSphere(float radius, int slices, int stacks) {
	GLUquadric *sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);
	gluSphere(sphere, radius, slices, stacks);
}
void drawTorus(float inner, float outer, int slices, int rings) {

	double pi = acos(-1.0);
	float vNormal[3];
	double majorStep = 2.0f * pi / slices;
	double minorStep = 2.0f * pi / rings;
	int i, j;
	for (i = 0; i < slices; ++i) {
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		GLfloat x0 = (GLfloat) cos(a0);
		GLfloat y0 = (GLfloat) sin(a0);
		GLfloat x1 = (GLfloat) cos(a1);
		GLfloat y1 = (GLfloat) sin(a1);
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j <= rings; ++j) {
			double b = j * minorStep;
			GLfloat c = (GLfloat) cos(b);
			GLfloat r = inner * c + outer;
			GLfloat z = inner * (GLfloat) sin(b);
			// First point
			glTexCoord2f((float) (i) / (float) (rings), (float) (j) / (float) (slices));
			vNormal[0] = x0 * c;
			vNormal[1] = y0 * c;
			vNormal[2] = z / inner;
			//gltNormalizeVector(vNormal);
			glNormal3fv(vNormal);
			glVertex3f(x0 * r, y0 * r, z);
			glTexCoord2f((float) (i + 1) / (float) (rings), (float) (j) / (float) (slices));
			vNormal[0] = x1 * c;
			vNormal[1] = y1 * c;
			vNormal[2] = z / inner;
			glNormal3f(vNormal[0], vNormal[1], vNormal[2]);
			glVertex3f(x1 * r, y1 * r, z);
		}
		glEnd();
	}
}
void drawPlane(int p) {

	GLfloat ctrlpoints[4][3] = { { -0.5, 0.0, -0.5 }, { 0.5, 0.0, -0.5 }, { -0.5, 0.0, 0.5 }, { 0.5, 0.0, 0.5 } };

	GLfloat nrmlcompon[4][3] = { { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } };

	GLfloat textpoints[4][2] = { { 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 1.0 }, { 1.0, 1.0 } };
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2, 0.0, 1.0, 6, 2, &ctrlpoints[0][0]);
	glMap2f(GL_MAP2_NORMAL, 0.0, 1.0, 3, 2, 0.0, 1.0, 6, 2, &nrmlcompon[0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, 2, 2, 0.0, 1.0, 4, 2, &textpoints[0][0]);

	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_MAP2_NORMAL);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glMapGrid2f(p, 0.0, 1.0, p, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, p, 0, p);

}
void drawSeagull() {
//	cout << "seagull" << endl;
	vector<float> aux(3);
	glScalef(0.3, 0.3, 0.3);
	//body
	while (1) {
		glPushMatrix();

		//bodyleft
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		drawCylinder(0.5, 0.05, 3, 200, 100);
		glPopMatrix();

		//bodyright
		glPushMatrix();
		glRotatef(-90, 0, 1, 0);
		drawCylinder(0.5, 0.05, 3, 200, 100);
		glPopMatrix();

		glPopMatrix();
		break;
	}
//	cout << "draw body" << endl;
	//wing1
	while (1) {
		glPushMatrix();

		glTranslatef(0, 0, -1.2);
		glRotatef(-90, 0, 1, 0);

		//wingmain
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		drawRectangle(-0.5, -1.0, 0.5, 1.0);
		drawRectangle(-0.5, 1.0, 0.5, -1.0);
		glPopMatrix();

		//wingTip1
		glPushMatrix();
		vector<vector<float> > tip1(3);
		aux[0] = -1;
		aux[1] = 0;
		aux[2] = 0.5;
		tip1[0] = aux;
		aux[0] = -3;
		aux[1] = -1;
		aux[2] = 0;
		tip1[1] = aux;
		aux[0] = -1;
		aux[1] = 0;
		aux[2] = -0.5;
		tip1[2] = aux;
		drawTriangle(tip1[0], tip1[1], tip1[2]);

		aux[0] = -1;
		aux[1] = 0;
		aux[2] = -0.5;
		tip1[0] = aux;
		aux[0] = -3;
		aux[1] = -1;
		aux[2] = 0;
		tip1[1] = aux;
		aux[0] = -1;
		aux[1] = 0;
		aux[2] = 0.5;
		tip1[2] = aux;
		drawTriangle(tip1[0], tip1[1], tip1[2]);

		glPopMatrix();

		glPopMatrix();
		break;
	}
//	cout << "draw wing1" << endl;
//wing2
	while (1) {
		glPushMatrix();
		glTranslatef(0, 0, 1.2);
		glRotatef(90, 0, 1, 0);

		//wingmain
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		drawRectangle(-0.5, -1.0, 0.5, 1.0);
		drawRectangle(-0.5, 1.0, 0.5, -1.0);
		glPopMatrix();

		//wingTip1
		glPushMatrix();
		vector<vector<float> > tip1(3);
		aux[0] = -1;
		aux[1] = 0;
		aux[2] = 0.5;
		tip1[0] = aux;
		aux[0] = -3;
		aux[1] = -1;
		aux[2] = 0;
		tip1[1] = aux;
		aux[0] = -1;
		aux[1] = 0;
		aux[2] = -0.5;
		tip1[2] = aux;
		drawTriangle(tip1[0], tip1[1], tip1[2]);

		aux[0] = -1;
		aux[1] = 0;
		aux[2] = -0.5;
		tip1[0] = aux;
		aux[0] = -3;
		aux[1] = -1;
		aux[2] = 0;
		tip1[1] = aux;
		aux[0] = -1;
		aux[1] = 0;
		aux[2] = 0.5;
		tip1[2] = aux;
		drawTriangle(tip1[0], tip1[1], tip1[2]);

		glPopMatrix();

		glPopMatrix();
		break;
	}
//	cout << "draw wing2" << endl;
//head
	while (1) {
		glPushMatrix();

		glTranslatef(2.8, 0, 0);

//justHead
		glPushMatrix();
		drawSphere(0.5, 200, 100);
		glPopMatrix();

//beak
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 0.35);
		glRotatef(180, 0, 1, 0);
		vector<float> cp;

		cp.push_back(-0.25);
		cp.push_back(-0.25);
		cp.push_back(0);

		cp.push_back(-0.25);
		cp.push_back(0);
		cp.push_back(0);

		cp.push_back(-0.25);
		cp.push_back(0.25);
		cp.push_back(0);

		cp.push_back(0);
		cp.push_back(-0.25);
		cp.push_back(0);

		cp.push_back(0);
		cp.push_back(0);
		cp.push_back(-5);

		cp.push_back(0);
		cp.push_back(0.25);
		cp.push_back(0);

		cp.push_back(0.25);
		cp.push_back(-0.25);
		cp.push_back(0);

		cp.push_back(0.25);
		cp.push_back(0);
		cp.push_back(0);

		cp.push_back(0.25);
		cp.push_back(0.25);
		cp.push_back(0);

		Graph::Node::Patch p(2, 10, 10, "fill", cp);

		drawPatch("fill", 2, 10, 10, cp, p.textpoints);
		glPopMatrix();

		glPopMatrix();
		break;
	}
//	cout << "draw head" << endl;
//tail
	while (1) {
		glPushMatrix();

		glTranslatef(-3, 0, 0);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 1, 0, 0);

		vector<vector<float> > v1;
		vector<vector<float> > v2;

		aux[0] = -1.5;
		aux[1] = -1.3;
		aux[2] = 0;
		v1.push_back(aux);
		aux[0] = -0;
		aux[1] = 1.3;
		aux[2] = 0;
		v1.push_back(aux);
		aux[0] = 1.5;
		aux[1] = -1.3;
		aux[2] = 0;
		v1.push_back(aux);

		aux[0] = 1.5;
		aux[1] = -1.3;
		aux[2] = 0;
		v2.push_back(aux);
		aux[0] = -0;
		aux[1] = 1.3;
		aux[2] = 0;
		v2.push_back(aux);
		aux[0] = -1.5;
		aux[1] = -1.3;
		aux[2] = 0;
		v2.push_back(aux);

		drawTriangle(v1[0], v1[1], v1[2]);
		drawTriangle(v2[0], v2[1], v2[2]);

		glPopMatrix();
		break;
	}
//	cout << "draw tail" << endl;
}
void AnfScene::drawFlag(Graph::Node::Flag* f) {

	f->shader->bind(velocity);
	drawPlane(100);
	f->shader->unbind();

}

void AnfScene::drawNode(Graph::Node* n, string appearencerefID, bool init) {

	glPushMatrix();
	if (!init) {
		if (n->currentAnim < n->anim.size()) {
			n->anim[n->currentAnim]->apply();
		}
	}

	glPushMatrix();
	if (n->displaylist && !init) {
		glCallList(n->list);
		return;
	}
	if (n->appearencerefID != "inherit" && n->appearencerefID.size() > 0) {
		appearances[n->appearencerefID].appearence->apply();
		appearencerefID = n->appearencerefID;
	} else if (appearencerefID != "") {
		appearances[appearencerefID].appearence->apply();
	}
	glMultMatrixf(n->matrix);
	for (unsigned int i = 0; i < n->rectangle.size(); ++i) {
		drawRectangle(n->rectangle[i].xy1[0], n->rectangle[i].xy1[1], n->rectangle[i].xy2[0], n->rectangle[i].xy2[1]);
	}
	for (unsigned int i = 0; i < n->triangle.size(); ++i) {
		drawTriangle(n->triangle[i].xyz1, n->triangle[i].xyz2, n->triangle[i].xyz3);
	}
	for (unsigned int i = 0; i < n->torus.size(); ++i) {
		drawTorus(n->torus[i].inner, n->torus[i].outer, n->torus[i].slices, n->torus[i].loops);
	}
	for (unsigned int i = 0; i < n->cylinder.size(); ++i) {
		drawCylinder(n->cylinder[i].base, n->cylinder[i].top, n->cylinder[i].height, n->cylinder[i].slices, n->cylinder[i].stacks);
	}
	for (unsigned int i = 0; i < n->sphere.size(); ++i) {
		drawSphere(n->sphere[i].radius, n->sphere[i].slices, n->sphere[i].stacks);
	}
	for (unsigned int i = 0; i < n->plane.size(); ++i) {
		GLfloat ctrlpoints[4][3] = { { -0.5, -0.5, 0.0 }, { -0.5, 0.5, 0.0 }, { 0.5, -0.5, 0.0 }, { 0.5, 0.5, 0.0 } };
		GLfloat nrmlcompon[4][3] = { { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
		GLfloat textpoints[4][2] = { { 0.0, 0.0 }, { 0.0, 1.0 }, { 1.0, 0.0 }, { 1.0, 1.0 } };
		drawPlane(n->plane[i].parts);
	}
	for (unsigned int i = 0; i < n->patch.size(); ++i) {
		drawPatch(n->patch[i].compute, n->patch[i].order, n->patch[i].partsU, n->patch[i].partsV, n->patch[i].controlpoints, n->patch[i].textpoints);
	}
	for (unsigned int i = 0; i < n->flag.size(); ++i) {
		drawFlag(&n->flag[i]);
	}
	for (int i = 0; i < n->seagull.size(); ++i) {
		drawSeagull();
	}
	for (unsigned int i = 0; i < n->descendant.size(); ++i) {
		drawNode(n->descendant[i], appearencerefID, init);
	}
	glPopMatrix();
	glPopMatrix();
}

void AnfScene::display() {
	if (globals.drawing.mode == 0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (globals.drawing.mode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	// ---- BEGIN Background, camera and axis setup

	// Clear image and depth buffer everytime we update the scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (initial < (int) camera_id.size())
		cameras[camera_id[initial]]->apply();
	else
		CGFscene::activeCamera->applyView();
	CGFapplication::activeApp->forceRefresh();
	// Initialize Model-View matrix as identity (no transformation

	// Apply transformations corresponding to the camera position relative to the origin

//	light0->draw();
	// Draw (and update) light
	glPushMatrix();
	for (unsigned int i = 0; i < lights.size(); ++i) {
//		cout << "luz" << endl;
		lights[i].cgfl->update();
		if (lights[i].marker)
			lights[i].cgfl->draw();
	}
	glPopMatrix();
	// Draw axis
//	axis.draw();

	// ---- END Background, camera and axis setup

	// ---- BEGIN feature demos

//	materialAppearance->apply();
	// scale down a bit
//	glScalef(0.2, 0.2, 0.2);

	// picking example, the important parts are the gl*Name functions
	// and the code in the associted PickInterface class
	// Example 1: simple naming
	defapp->apply();
	game->draw();
	drawNode(&graph.nodes[graph.rootid], "", false);
	glPopMatrix();

	// ---- END feature demos

	// glutSwapBuffers() will swap pointers so that the back buffer becomes the front buffer and vice-versa
	glutSwapBuffers();
}

AnfScene::~AnfScene() {

}

void AnfScene::createList(Graph::Node* n, string appearencerefID) {

	if (n->displaylist) {
		cout << "List with number: " << listNumber << endl;
		n->list = listNumber;
		glNewList(listNumber, GL_COMPILE);
		listNumber++;
		drawNode(n, appearencerefID, true);
		glEndList();
	} else {
		if (n->appearencerefID != "inherit" && n->appearencerefID.size() > 0)
			appearencerefID = n->appearencerefID;
		for (int i = 0; i < (int) n->descendant.size(); ++i) {
			createList(n->descendant[i], appearencerefID);
		}
	}

}

void AnfScene::update(unsigned long t) {
	game->update(t);
	this->t = t;
	for (int i = 0; i < shaders.size(); ++i) {
		shaders[i]->update(t);
	}
	map<string, Graph::Node>::iterator it = graph.nodes.begin();
	while (it != graph.nodes.end()) {
		if (it->second.currentAnim < it->second.anim.size())
			if (it->second.anim[it->second.currentAnim]->update(t))
				it->second.currentAnim += 1;
		it++;
	}
}
