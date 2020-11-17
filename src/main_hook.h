/**
 * This macro must be placed once in main() before any haxe-generated classes are used.
 */
#define HAXECPP_MAIN_HOOK \
		HX_TOP_OF_STACK \
		hx::Boot(); \
		__boot_all();
