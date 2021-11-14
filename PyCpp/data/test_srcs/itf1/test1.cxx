
///@module mymodule.test_unders

#include <iostream>

class Player  {

	constructor() {{
	    life_points = 0;
	    mana = 0;
    }}

    constructor(const int &a, const int &b) {{
        life_points = a;
        mana = b;
    }}

	const bool alive () {{
	    return (life_points > 0);
	}}

	void make_mana () {{
	    mana++;
	}}

	int life_points;
	int mana;

} ;
