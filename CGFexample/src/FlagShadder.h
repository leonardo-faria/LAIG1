/*
 * Flag.h
 *
 *  Created on: 19/11/2014
 *      Author: leonardo
 */
#include "CGFshader.h"
#include "CGFtexture.h"
#ifndef FLAG_H_
#define FLAG_H_

class FlagShadder: public CGFshader {
public:
	virtual void bind(float v);
	virtual void unbind();

	void update(unsigned long t);

	FlagShadder(string t);
	virtual ~FlagShadder();
	float angle;
	unsigned long startTime;
	float time;
	int angleLoc;
	int timeLoc;
	int vLoc;
	int baseTextLoc;
	CGFtexture* text;
	string t;
};

#endif /* FLAG_H_ */
