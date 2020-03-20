/* C/C++ ABI wrapper for NQC's rcxlib */

#if defined(__cplusplus)
extern "C" {
#endif

void *rcx_pipe_init();
void rcx_pipe_close(void *);
long rcx_pipe_read(void *, void *, long, long);
long rcx_pipe_write(void *, const void *, long);
long rcx_pipe_send(void *, void *, int, void *, int, int, int, int);

#if defined(__cplusplus)
}
#endif
