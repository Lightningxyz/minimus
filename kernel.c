void kernel_main()
{
    char *video = (char*) 0xb8000;

    const char *msg = "Hello from easy OS";

    for (int i = 0; msg[i] != '\0'; i++)
    {
        video[i * 2] = msg[i];
        video[i * 2 + 1] = 0x0F;
    }

    while (1);
}