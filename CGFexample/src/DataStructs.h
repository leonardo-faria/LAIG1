#include <vector>
#include <map>
#include <iostream>
#include <math.h>
#include "FlagShadder.h"
#include "CGFapplication.h"
using namespace std;
class Globals {
public:
	class Drawing {
	public:
		int mode;
		string shadding;
		vector<float> background;
	};

	class Culling {
	public:
		string face;
		string order;
	};

	class Lighting {
	public:
		bool doublesided, local, enabled;
		vector<float> ambient;
	};
	Globals() {
		drawing.mode = 0;
		drawing.shadding = "gourand";
		for (int i = 0; i < 4; ++i)
			drawing.background.push_back(0);
		culling.face = "back";
		culling.order = "ccw";
		for (int i = 0; i < 4; ++i)
			lighting.ambient.push_back(0);
		lighting.doublesided = false;
		lighting.enabled = true;
		lighting.local = true;
	}
	Drawing drawing;
	Culling culling;
	Lighting lighting;
};

class Light {
public:
	bool marker, enabled;
	CGFlight* cgfl;
};

class Textures {
public:
	class Texture {
	public:
		Texture() {
			file = "";
		}
		string file;
		float texlength_s, texlength_t;
	};
	map<string, Texture> texture;
};

class Appearence {
public:
	CGFappearance* appearence;
	string textureref;
};

class Animation {
public:
	virtual bool update(unsigned long t)=0;
	virtual void apply()=0;

	float time;
};

class LinearAnimation: public Animation {
public:
	vector<vector<float> > dir;
	vector<float> time;
	int currentDir;
	vector<vector<float> > pos;
	float v, t, start;

	bool update(unsigned long ti) {
		if (start == 0)
			start = ti;
		unsigned long t = ti - start;
		if (t / 1000.0 > time[currentDir]) {
			if (currentDir < dir.size()) {
				cout << "CHANGE" << endl;
			}
			currentDir++;
		}
		this->t = t;
		if (currentDir >= time.size())
			return true;
		return false;
	}
	void apply() {
		if (v == 0)
			return;
		float inc;
		if (currentDir - 1 < 0)
			inc = v * (t / 1000.0 - 0);
		else
			inc = v * (t / 1000.0 - time[currentDir - 1]);
		float product;
		if (dir[currentDir][1] != 0) {
			if (dir[currentDir][2] == 0)
				product = 1;
			else if (dir[currentDir][2] < 0)
				product = -1;
		} else
			product = dir[currentDir][2];
		float angle = acos(product);

		angle = angle * 180.0 / acos(-1);
		if (dir[currentDir][0] < 0)
			angle *= -1;
		glTranslatef(pos[currentDir][0] + dir[currentDir][0] * inc, pos[currentDir][1] + dir[currentDir][1] * inc, pos[currentDir][2] + dir[currentDir][2] * inc);
		glRotatef(angle, 0, 1, 0);
	}
};

class CircularAnimation: public Animation {
public:
	vector<float> center;
	float sang, t, start, vang, r;
	bool update(unsigned long ti) {
		if (start == 0)
			start = ti;
		unsigned long t = ti - start;
		this->t = t;
		if (t / 1000.0 > time)
			return true;
		return false;
	}
	void apply() {
		glTranslatef(center[0], center[1], center[2]);
		glRotatef(sang + vang * (t / 1000.0), 0, 1, 0);
		glTranslatef(r, 0, 0);
	}
};

class Graph {
public:
	class Node {
	public:
		class Rectangle {
		public:
			vector<float> xy1, xy2;
		};
		class Triangle {
		public:
			vector<float> xyz1, xyz2, xyz3;
		};
		class Cylinder {
		public:
			float base, top, height;
			int slices, stacks;
		};
		class Sphere {
		public:
			float radius;
			int slices, stacks;
		};
		class Torus {
		public:
			float inner, outer;
			int slices, loops;
		};
		class Plane {
		public:
			Plane(int p) {
				parts = p;
			}
			int parts;
		};
		class Patch {
		public:
			int order, partsU, partsV;
			string compute;
			vector<GLfloat> controlpoints, textpoints;
			Patch(int order, int partsU, int partsV, string compute, vector<GLfloat> controlpoints) {
				this->compute = compute;
				this->order = order;
				this->partsU = partsU;
				this->partsV = partsV;
				this->controlpoints = controlpoints;
				for (int i = 0; i < order + 1; i++) {
					for (int j = 0; j < order + 1; j++) {
						textpoints.push_back((float) i / (float) order);
						textpoints.push_back((float) j / (float) order);
					}
				}
			}
		};
		class Flag {
		public:
			FlagShadder* shader;
			Plane* plane;
			string text;
			Flag() {
				plane = new Plane(100);
				shader = new FlagShadder();
			}
		};
		float matrix[16];
		bool displaylist;
		int list;
		string appearencerefID;
		vector<Rectangle> rectangle;
		vector<Triangle> triangle;
		vector<Cylinder> cylinder;
		vector<Sphere> sphere;
		vector<Torus> torus;
		vector<Plane> plane;
		vector<Patch> patch;
		vector<Flag> flag;

		int currentAnim;
		vector<Animation*> anim;
		vector<Node*> descendant;
		Node() {
			list = 0;
			displaylist = false;
			for (int i = 0; i < 16; ++i)
				matrix[i] = 0;
		}
	};
	map<string, Node> nodes;
	string rootid;
};

class Camera: public CGFcamera {
public:
	string id;
	virtual void apply() = 0;
};
class CPerspective: public Camera {
public:
	float near, far, angle, pos[3], target[3];
	void apply() {
		float ratio = ((float) CGFapplication::width) / ((float) CGFapplication::height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(angle, ratio, near, far);
		gluLookAt(pos[0], pos[1], pos[2], target[0], target[1], target[2], 0, 1, 0);
	}
};
class COrtho: public Camera {
public:
	string direction;
	float near, far, left, right, top, bottom;
	void apply() {
		float ratio = ((float) CGFapplication::width) / ((float) CGFapplication::height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(left * ratio, right * ratio, bottom, top, near, far);
		if (direction == "x")
			glRotatef(-90, 0, 1, 0);
		if (direction == "y")
			glRotatef(90, 1, 0, 0);
	}

};
