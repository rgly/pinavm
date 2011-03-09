#ifndef LIBTEST_H
#define LIBTEST_H


class base {
public:
	virtual void hello();
};

class derived : public base {
public:
	void hello();
	void hello_derived();
};

void simple_in_native(base *b);

#define TRY_CAST(base, name, target) do {				\
		target	*t = dynamic_cast<target *>(base);	\
		if (!t) {						\
			cout << "FAILED:  dynamic_cast from " name " to " #target << endl; \
		} else {						\
			cout << "SUCCESS: dynamic_cast from " name " to " #target  << endl; \
		}							\
	} while (0);

class target {
public:
	virtual void hello_target();
};

class base1 {
public:
	virtual void hello1() {};
};

class derived_common1 : public base1 {
	
};

class base2 {
public:
	virtual void hello2();
};

class derived_common2 : public base2, public target {
	
};


class target_nv {
public:
	void hello_target();
	virtual void hello_target_virtual() {};
};

class base_nv1 {
public:
	virtual void hello1() {};
};

class derived_common_nv1 : public base_nv1, public target_nv {
	
};

class base_nv2 {
public:
	virtual void hello2();
};

class derived_common_nv2 : public base_nv2, public target_nv {
	
};

void common_base_in_native(base1 *b1, base2 *b2, base_nv1 *nv1, base_nv2 *nv2);

#endif // LIBTEST_H
