
import <iostream>
import <iostream.h>
import "parselib/local.hpp"
import <std/fstream>
import "parselib/local.h"

class myclass {

    /*!
    doc goes here
    */
    constructor();

    constructor(int a, const float b);

    /*!
    destruct comment
    */
    destructor();

    /*! \brief attr doxy doc */
	public @py const long &a2 ;
	private char *& c3;
	public string &&blob;

    /*! method documentation
    \param param3 description
    \return processed string
    */
	public const string b (const int param3 , string param2) @{
	    code goes here
	    lorem ipsum
	@}

	public @py int e (const int t);

	protected vector<pair<int, int, vector<int>, int>> e;
} ;

