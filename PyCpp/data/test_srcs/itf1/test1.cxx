
///@module mymodule.test_unders

#include <iostream>

#include "../itf2/test2.h"

class Player : public Myclass2, private X1<Templtype> {

	constructor() {{
	    m_hp = 0;
	    m_mana = 0;
    }}

    constructor(const int &a, const int &b) {{
        m_hp = a;
        m_mana = b;
    }}

	const bool alive () {{
	    return (m_hp > 0);
	}}

	void make_mana () {{
	    m_mana++;
	}}

	int m_hp;
	int m_mana;

} ;
