/*
 * PuTTY version numbering
 */

#define STR1(x) #x
#define STR(x) STR1(x)
#ifdef ATTAC_CODE
	#if defined SNAPSHOT

	char ver[] = "Development snapshot " STR(SNAPSHOT)  __TIME__;
	char sshver[] = "ATTAC-Snapshot-" STR(SNAPSHOT)  __TIME__;

	#elif defined RELEASE

	char ver[] = "Release " STR(RELEASE) " (" __DATE__ " " __TIME__ ")";
	char sshver[] = "ATTAC-Release-" STR(RELEASE);

	#else


	char ver[] = "Unidentified build, " __DATE__ " " __TIME__;
	char sshver[] = "ATTAC-Local: " __DATE__ " " __TIME__;


	#endif

	char releaseNumber[] = STR(NUMBERRELEASE);
	char majorNumber[] = STR(NUMBERMAJOR);
	char minorNumber[] = STR(NUMBERMINOR);

	char reginaver[] = STR(REGINARELEASE);
#else
	#if defined SNAPSHOT

	char ver[] = "Development snapshot " STR(SNAPSHOT);
	char sshver[] = "PuTTY-Snapshot-" STR(SNAPSHOT);

	#elif defined RELEASE

	char ver[] = "Release " STR(RELEASE);
	char sshver[] = "PuTTY-Release-" STR(RELEASE);

	#else


	char ver[] = "Unidentified build, " __DATE__ " " __TIME__;
	char sshver[] = "PuTTY-Local: " __DATE__ " " __TIME__;


	#endif
#endif

/*
 * SSH local version string MUST be under 40 characters. Here's a
 * compile time assertion to verify this.
 */
enum { vorpal_sword = 1 / (sizeof(sshver) <= 40) };
