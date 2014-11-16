#include <vector>
#include <map>
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
		float matrix[16];
		bool displaylist;
		int list;
		string appearencerefID;
		vector<Rectangle> rectangle;
		vector<Triangle> triangle;
		vector<Cylinder> cylinder;
		vector<Sphere> sphere;
		vector<Torus> torus;
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
		float ratio = ((float) CGFapplication::width)
				/ ((float) CGFapplication::height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(angle, ratio, near, far);
		gluLookAt(pos[0], pos[1], pos[2], target[0], target[1], target[2], 0, 1,
				0);
	}
};
class COrtho: public Camera {
public:
	string direction;
	float near, far, left, right, top, bottom;
	void apply() {
		float ratio = ((float) CGFapplication::width)
				/ ((float) CGFapplication::height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(left * ratio, right * ratio, bottom, top, near, far);
		if (direction == "x")
			glRotatef(-90, 0, 1, 0);
		if (direction == "y")
			glRotatef(90, 1, 0, 0);
	}

};

//virtual class Animation {
//public:
//	int time;
//};
//
//class LinearAnimation: public Animation {
//public:
//	vector<int[3]> cpoints;
//};
//
//class CircularAnimation: public Animation {
//public:
//
//};
