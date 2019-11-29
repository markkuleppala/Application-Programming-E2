
int main(void) {
    int stdout_fd = dup(STDOUT_FILENO);
    freopen(redirected_filename, "w", stdout);
    fclose(stdout);
    dup2(stdout_fd, STDOUT_FILENO);
    stdout = fdopen(STDOUT_FILENO, "w");
    close(stdout_fd);
}