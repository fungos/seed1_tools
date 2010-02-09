 #ifndef __ARGS_H__
#define __ARGS_H__

typedef void (*CallbackFunc)(const char *argValue, void *data);

class Args
{
	friend class CmdLineParser;

	private:
		const char *shrt;
		const char *lng;
		bool exclusive;
		bool depends;
		bool nullAllow;
		CallbackFunc callback;
		void *data;

		Args(const Args &);
		bool operator=(const Args &);
	public:
		Args(const char *shortStr, const char *longStr, const bool nullAllow, const bool exclusive, const bool dependsOnValue, CallbackFunc func, void *data);
		~Args();
};

#endif
