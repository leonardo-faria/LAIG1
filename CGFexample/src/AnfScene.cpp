#include "AnfScene.h"
#include "CGFaxis.h"
#include "CGFapplication.h"
#include <math.h>
#include <iostream>

float pi = acos(-1.0);
float deg2rad = pi / 180.0;

#define BOARD_HEIGHT 6.0
#define BOARD_WIDTH 6.4

AnfScene::AnfScene(string filename) {
	TiXmlElement* globalsElement;
	TiXmlElement* camerasElement;
	TiXmlElement* lightsElement;
	TiXmlElement* textsElement;
	TiXmlElement* appearsElement;
	TiXmlElement* graphElement;

	TiXmlDocument *doc = new TiXmlDocument(filename.c_str());

	if (!doc->LoadFile()) {
		printf("Could not load file '%s'. Error='%s'. Exiting.\n", filename.c_str(), doc->ErrorDesc());
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
	graphElement = anfElement->FirstChildElement("graph");

	printf("Starting globals\n");
	if (globalsElement == NULL)
		printf("Globals block not found!\n");
	else {
		TiXmlElement* drawingElement = globalsElement->FirstChildElement("drawing");
		char *mode, *shading;
		float background[4];
		mode = (char*) drawingElement->Attribute("mode");
		shading = (char*) drawingElement->Attribute("shading");
		sscanf((char*) drawingElement->Attribute("background"), "%f %f %f %f", &background[0], &background[1], &background[2], &background[3]);

		globals.drawing.mode = mode;
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
		for (int i = 0; i < 4; ++i)
			globals.lighting.ambient.push_back(ambient[i]);
	}

	printf("Read globals\nStarting Cameras\n");

	if (camerasElement == NULL)
		printf("Cameras block not found!\n");
	else {
		char* initial;
		initial = (char*) camerasElement->Attribute("initial");
		cameras.initial = initial;
		for (TiXmlElement* p = camerasElement->FirstChildElement("perspective"); p != NULL; p = p->NextSiblingElement("perspective")) {
			char* id;
			float near, far, angle;
			float pos[3], target[3];
			id = (char*) p->Attribute("id");
			p->QueryFloatAttribute("near", &near);
			p->QueryFloatAttribute("far", &far);
			p->QueryFloatAttribute("angle", &angle);
			sscanf(p->Attribute("pos"), "%f %f %f", &pos[0], &pos[1], &pos[2]);
			sscanf(p->Attribute("target"), "%f %f %f", &target[0], &target[1], &target[2]);
			Cameras::Perspective perspective;
			perspective.angle = angle;
			perspective.near = near;
			perspective.far = far;
			for (int i = 0; i < 3; ++i)
				perspective.pos.push_back(pos[i]);
			for (int i = 0; i < 3; ++i)
				perspective.target.push_back(target[i]);
			cameras.camera[id] = perspective;

		}
		for (TiXmlElement* o = camerasElement->FirstChildElement("ortho"); o != NULL; o = o->NextSiblingElement("ortho")) {
			char *id, *direction;
			float near, far, left, right, top, bottom;
			id = (char*) o->Attribute("id");
			direction = (char*) o->Attribute("direction");
			o->QueryFloatAttribute("near", &near);
			o->QueryFloatAttribute("far", &far);
			o->QueryFloatAttribute("left", &left);
			o->QueryFloatAttribute("right", &right);
			o->QueryFloatAttribute("top", &top);
			o->QueryFloatAttribute("bottom", &bottom);
			Cameras::Ortho ortho;
			ortho.bottom = bottom;
			ortho.direction = *direction;
			ortho.far = far;
			ortho.left = left;
			ortho.near = near;
			ortho.right = right;
			ortho.top = top;
			cameras.camera[id] = ortho;
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
		int i = 0;

		for (TiXmlElement* l = lightsElement->FirstChildElement("light"); l != NULL; l = l->NextSiblingElement("light")) {

			Light light;
			char *type;
			type = (char*) l->Attribute("type");
			float pos[4];
			sscanf(l->Attribute("pos"), "%f %f %f", &pos[0], &pos[1], &pos[2]);
			pos[3] = 1;

			if (strcmp(type, "spot") == 0) {
				float target[3], angle, exponent;
				l->QueryFloatAttribute("angle", &angle);
				l->QueryFloatAttribute("exponent", &exponent);
				sscanf(l->Attribute("target"), "%f %f %f", &target[0], &target[1], &target[2]);
				float unit = sqrt(target[0] * target[0] + target[1] * target[1] + target[2] * target[2]);
				for (int i = 0; i < 3; i++) {
					target[i] = target[i] / unit;
				}
				glLightf(idl[i], GL_SPOT_CUTOFF, angle);
				glLightf(idl[i], GL_SPOT_EXPONENT, exponent);
				glLightfv(idl[i], GL_SPOT_DIRECTION, target);

			}
			light.cgfl = new CGFlight(idl[i], pos);
			++i;
			bool enabled, marker;
			l->QueryBoolAttribute("enabled", &enabled);
			l->QueryBoolAttribute("marker", &marker);

			for (TiXmlElement* c = l->FirstChildElement("component"); c != NULL; c = c->NextSiblingElement("component")) {
				char* type = (char*) c->Attribute("type");
				float value[4];
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

			if (strcmp(type, "spot") == 0)
				light.cgfl->enable();
			else
				light.cgfl->enable();
			light.cgfl->update();
			lights.push_back(light);
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
			file = (char*) t->Attribute("file");
			t->QueryFloatAttribute("texlength_s", &textlength_s);
			t->QueryFloatAttribute("texlength_t", &textlength_t);
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
			a->QueryFloatAttribute("shininess", &shininess);
			id = (char*) a->Attribute("id");

			textureref = (char*) a->Attribute("textureref");
			float ambient[4], diffuse[4], specular[4];
			for (TiXmlElement* c = a->FirstChildElement("component"); c != NULL; c = c->NextSiblingElement("component")) {
				char* type = (char*) c->Attribute("type");
				float value[4];
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
			CGFappearance *ap = new CGFappearance(ambient, specular, diffuse, shininess);
			Appearence app;
			app.appearence = ap;
			if (textureref != NULL) {
				app.textureref = textureref;
				ap->setTexture(textures.texture[textureref].file);
			}
			appearances[id] = app;
		}
	}

	printf("Read appearences\nStarting graph\n");

	if (graphElement == NULL)
		printf("Graph block not found!\n");
	else {
		char *rootid = (char*) graphElement->Attribute("rootid");
		graph.rootid = rootid;

		for (TiXmlElement* n = graphElement->FirstChildElement("node"); n != NULL; n = n->NextSiblingElement("node")) {

			Graph::Node node;
			char* id = (char*) n->Attribute("id");
			glLoadIdentity();
			TiXmlElement* transformElement = n->FirstChildElement("transforms");
			for (TiXmlElement* t = transformElement->FirstChildElement("transform"); t != NULL; t = t->NextSiblingElement("transform")) {

				char* type;
				type = (char*) t->Attribute("type");

				if (strcmp(type, "translate") == 0) {
					float to[3];
					sscanf(t->Attribute("to"), "%f %f %f", &to[0], &to[1], &to[2]);
					glTranslatef(to[0], to[1], to[2]);
				} else if (strcmp(type, "rotate") == 0) {
					string axis;
					float angle;
					axis = (char*) t->Attribute("axis");
					t->QueryFloatAttribute("angle", &angle);
					glRotatef(angle, axis == "x", axis == "y", axis == "z");
				} else {
					float factor[3];
					sscanf(t->Attribute("factor"), "%f %f %f", &factor[0], &factor[1], &factor[2]);
					glScalef(factor[0], factor[1], factor[2]);
				}
			}
			glGetFloatv(GL_MODELVIEW_MATRIX, node.matrix);
			TiXmlElement* apElement = n->FirstChildElement("appearanceref");
			char* appearancerefId = (char*) apElement->Attribute("id");
			node.appearencerefID = appearancerefId;

			TiXmlElement* primitivesElement = n->FirstChildElement("primitives");
			if (primitivesElement != NULL) {
				for (TiXmlElement* p = primitivesElement->FirstChildElement("rectangle"); p != NULL; p = p->NextSiblingElement("rectangle")) {
					float xy1[2], xy2[2];
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
					p->QueryFloatAttribute("base", &base);
					p->QueryFloatAttribute("top", &top);
					p->QueryFloatAttribute("height", &height);
					p->QueryIntAttribute("slices", &slices);
					p->QueryIntAttribute("stacks", &stacks);
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
					p->QueryFloatAttribute("radius", &radius);
					p->QueryIntAttribute("slices", &slices);
					p->QueryIntAttribute("stacks", &stacks);
					Graph::Node::Sphere s;
					s.radius = radius;
					s.slices = slices;
					s.stacks = stacks;
					node.sphere.push_back(s);
				}
				for (TiXmlElement* p = primitivesElement->FirstChildElement("torus"); p != NULL; p = p->NextSiblingElement("torus")) {
					float inner, outer;
					int slices, loops;
					p->QueryFloatAttribute("inner", &inner);
					p->QueryFloatAttribute("outer", &outer);
					p->QueryIntAttribute("slices", &slices);
					p->QueryIntAttribute("loops", &loops);
					Graph::Node::Torus t;
					t.inner = inner;
					t.outer = outer;
					t.slices = slices;
					t.loops = loops;
					node.torus.push_back(t);
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
	for (int i = 0; i < 4; i++)
		ambientLight[i] = globals.lighting.ambient[i];

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	if (globals.drawing.shadding == "gouraud")
		glShadeModel(GL_SMOOTH);
	else if (globals.drawing.shadding == "flat")
		glShadeModel(GL_FLAT);

	if (globals.drawing.mode == "fill")
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (globals.drawing.mode == "line")
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (globals.drawing.mode == "point")
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	glClearColor(globals.drawing.background[0], globals.drawing.background[1], globals.drawing.background[2], globals.drawing.background[3]);
	glNormal3f(0, 0, 1);

	setUpdatePeriod(30);
}

void drawRectangle(float x1, float y1, float x2, float y2) {
	glBegin(GL_POLYGON);
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
	glVertex3f(v2[0], v2[1], v3[2]);
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

	int i, j;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	GLfloat ringDelta, sideDelta;

	ringDelta = 2.0 * M_PI / rings;
	sideDelta = 2.0 * M_PI / slices;

	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;
	for (i = rings - 1; i >= 0; i--) {
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		glBegin(GL_QUAD_STRIP);
		phi = 0.0;
		for (j = slices; j >= 0; j--) {
			GLfloat cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			dist = outer + inner * cosPhi;

			glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
			glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, inner * sinPhi);
			glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
			glVertex3f(cosTheta * dist, -sinTheta * dist, inner * sinPhi);
		}
		glEnd();
		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
	}

}

void AnfScene::drawNode(Graph::Node* n) {
	glPushMatrix();

	if (n->appearencerefID != "inherit" && n->appearencerefID.size() > 0)
		appearances[n->appearencerefID].appearence->apply();
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
	for (unsigned int i = 0; i < n->descendant.size(); ++i) {
		drawNode(n->descendant[i]);
	}
	glPopMatrix();
}

void AnfScene::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CGFscene::activeCamera->applyView();

	for (int i = 0; i < lights.size(); ++i) {
		lights[i].cgfl->update();
		if (lights[i].marker)
			lights[i].cgfl->draw();
	}
	axis.draw();
	drawNode(&graph.nodes[graph.rootid]);

	glutSwapBuffers();
}

AnfScene::~AnfScene() {

}
