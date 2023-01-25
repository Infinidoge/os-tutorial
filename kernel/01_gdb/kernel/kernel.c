/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */
void dummy_test_entrypoint() {
}
// use _start instead of main, kinda like begin for arduino
void _start() {
	char* video_memory = (char*) 0xb8000;
	*video_memory = 'X';
}
