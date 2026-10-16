#pragma once
#include <csignal>


volatile sig_atomic_t stop = 0;
void handleSignal(int signal);
int main();