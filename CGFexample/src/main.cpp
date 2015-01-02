/* 
 * G0_Base: projeto inicial de CGra
 * 
 */

#include <iostream>
#include <exception>

#include "Interface.h"
#include "PickInterface.h"


using std::cout;
using std::exception;

int main(int argc, char* argv[]) {

	CGFapplication app = CGFapplication();

	try {
		app.init(&argc, argv);
//		if (argc < 2) {
//			printf("currect use: ./cgfexample <anf destination>\n");
//			exit(1);
//		}s
		app.setScene(new AnfScene((char*) "LAIG_TP1_ANF_T01_G03_v2.anf"));
//		app.setInterface(new Interface());
		app.setInterface(new PickInterface());

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
