
///@module mymodule.test_unders

#include <iostream>

#include "../itf2/test2.h"

class Player : public Myclass2 {

	constructor() {{
	    life_points = 0;
	    m_mana = 0;
    }}

    constructor(const int &a, const int &b) {{
        life_points = a;
        m_mana = b;
    }}

	const bool alive () {{
	    return (life_points > 0);
	}}

	void make_mana () {{
	    m_mana++;
	}}

	int life_points;
	int mana;

} ;
