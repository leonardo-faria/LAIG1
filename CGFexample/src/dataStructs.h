#include <string>
#include <map>
using namespace std;
class drawingSt {
public:
	string mode;
	string shading;
	float background[4];
};
class cullingSt {
public:
	string face;
	string order;
};
class lightingSt {
public:
	bool doublesided;
	bool local;
	bool enabled;
	float ambient[4];
};

class cameraSt {
public:
	string id;

	cameraSt() {
		string id = "null";
	}
};
class persCamSt: public cameraSt {
public:

	float near, far, angle;
	float* pos, *target;
	persCamSt(string id, float near, float far, float angle, float* pos,
			float* target) {
		this->id = id;
		this->near = near;
		this->angle = angle;
		this->near = near;
		this->far = far;
		this->pos = pos;
		this->target = target;

	}
};

class orthoCamSt: public cameraSt {
public:
	string direction;
	float near, far, left, right, top, bottom;
	orthoCamSt(string id, string direction, float near, float far, float left,
			float right, float top, float bottom) {
		this->id = id;
		this->direction = direction;
		this->near = near;
		this->far = far;
		this->left = left;
		this->right = right;
		this->top = top;
		this->bottom = bottom;

	}
};
class cameraMp {
public:
	cameraSt* iniCam;
	map<string, cameraSt*> cameras;
	cameraSt* getCamera(string id) {
		cameraSt* ret;
		if ((cameras.find(id)) == cameras.end()) {
			ret = new cameraSt();
			cameras.insert(pair<string, cameraSt*>(id, ret));
		} else
			ret = cameras.find(id)->second;
		return ret;
	}
	void addOrthoCam(string id, string direction, float near, float far,
			float left, float right, float top, float bottom) {
		if ((cameras.find(id)) == cameras.end()) {

			cameras.insert(
					pair<string, cameraSt*>(id,
							new orthoCamSt(id, direction, near, far, left,
									right, top, bottom)));
		} else {
			cameras.find(id)->second = new orthoCamSt(id, direction, near, far,
					left, right, top, bottom);
		}
	}
	void addPrespCam(string id, float near, float far, float angle, float* pos,
			float* target) {
		if ((cameras.find(id)) == cameras.end()) {

			cameras.insert(
					pair<string, cameraSt*>(id,
							new persCamSt(id, near, far, angle, pos, target)));
		} else {
			cameras.find(id)->second = new persCamSt(id, near, far, angle, pos,
					target);
		}
	}

};
class lightSt {
public:
	string id;
	bool enabled, marker;
	float* pos;
	float* ambient, *diffuse, *specular;
};
class lightOmniSt: public lightSt {
public:
	lightOmniSt(string id, bool enabled, bool marker, float* pos,
			float* ambient, float* diffuse, float* specular) {
		this->id = id;
		this->enabled = enabled;
		this->marker = marker;
		this->pos = pos;
		this->ambient = ambient;
		this->specular = specular;
		this->diffuse = diffuse;

	}
};
class lightSpotSt: public lightSt {
public:
	float angle, exponent;
	float *target;
	lightSpotSt(string id, bool enabled, bool marker, float* pos, float angle,
			float* target, float exponent, float* ambient, float* diffuse,
			float* specular) {
		this->id = id;
		this->enabled = enabled;
		this->marker = marker;
		this->pos = pos;
		this->angle = angle;
		this->target = target;
		this->exponent = exponent;
		this->ambient = ambient;
		this->specular = specular;
		this->diffuse = diffuse;
	}
};
class lightMp {
public:
	map<string, lightSt*> lights;
	lightSt* getCamera(string id) {
		lightSt* ret;

		ret = lights.find(id)->second;
		return ret;
	}
	void addLightOmni(string id, bool enabled, bool marker, float* pos,
			float* ambient, float* diffuse, float* specular) {
		lights.insert(
				pair<string, lightSt*>(id,
						new lightOmniSt(id, enabled, marker, pos, ambient,
								diffuse, specular)));

	}
	void addLightSpot(string id, bool enabled, bool marker, float* pos,
			float angle, float* target, float exponent, float* ambient,
			float* diffuse, float* specular) {
		lights.insert(
				pair<string, lightSt*>(id,
						new lightSpotSt(id, enabled, marker, pos, angle, target,
								exponent, ambient, diffuse, specular)));

	}

};

class textureSt {
public:
	string id, file;
	float texlenght_s, texlenght_t;
	textureSt(string id, string file, float texlenght_s, float texlenght_t) {
		this->id = id;
		this->file = file;
		this->texlenght_s = texlenght_s;
		this->texlenght_t = texlenght_t;

	}
};
class textureMp {
public:
	map<string, textureSt*> textures;
	textureSt* getCamera(string id) {
		textureSt* ret;

		ret = textures.find(id)->second;
		return ret;
	}
	void addTexture(string id, string file, float texlenght_s,
			float texlenght_t) {
		textures.insert(
				pair<string, textureSt*>(id,
						new textureSt(id, file, texlenght_s, texlenght_t)));

	}

};
class appearanceSt {
public:
	string id, textureref;
	float shininess;
	float ambient[4], diffuse[4], specular[4];
};

class tanslateST {
public:
	float to[3];
};

class rotateSt {
public:
	char axis;
	float angle;
};

class scaleSt {
public:
	float factor[3];
};

class rectangleSt {
public:
	float xy1[2], xy2[2];

};

class triangleSt {
public:
	float xyz1[3], xyz2[3], xyz3[3];
};

class cylinderSt {
public:
	float base, top, height;
	int slices, stacks;
};

class sphereSt {
public:
	float radius;
	int slices, stacks;
};

class torusSt {
public:
	float inner, outer;
	int slices, stacks;
};

