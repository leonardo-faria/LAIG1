/* 
 * G0_Base: projeto inicial de CGra
 * 
 */

#include <iostream>
#include <exception>

#include "Interface.h"
#include "CGFapplication.h"

#include "AnfScene.h"

using std::cout;
using std::exception;

int main(int argc, char* argv[]) {

	CGFapplication app = CGFapplication();

	try {
		app.init(&argc, argv);
		app.setScene(new AnfScene("shipwreck.anf"));
		app.setInterface(new Interface());

		app.run();
	} catch (GLexception& ex) {
		cout << "Error: " << ex.what();
		return -1;
	} catch (exception& ex) {
		cout << "Unexpected error: " << ex.what();
		return -1;
	}

	return 0;
}
