
module mymodule.test_unders ;

#include <iostream>
#include "stdio.h"
#include <boost/filesystem.hpp>

class myclass :
	public absclass<
		abstracinexistanttype&,
		someptrTemplate*
	>,
	private xbezglife
{

	constructor();

    constructor(int a, const float b);

	const string b (const int param3 , string param2) @{
		code goes here01
		lorem ipsum
		if (cond) {
			block;
		}
	@}

	public void proc ();

	public @py int e (const int t);

	vector<pair<int*, int**, vector<int>, int>> e;
} ;