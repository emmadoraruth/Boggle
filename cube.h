struct cube{
	char face; //The letter on the cube's face
	int checked; //Used to mark whether the cube has been used in the search for a word
};

char get_face(struct cube *c);

int is_checked(struct cube *c);

void set_face(struct cube *c, char letter);

void check(struct cube *c);

void uncheck(struct cube *c);

struct cube * init_cube(char my_face);
