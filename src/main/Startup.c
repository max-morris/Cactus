
int Cactus_Startup(void)
{
  
  const char *string;

  string = "        \\ |/\n        /--\\\n      \\|   .|-                    \\  |  |  |  /  \n       | .  |\n       | .  |-                   -  Cactus 4.0  -\n      -|    |\n       |  / |                     /  |  |  |  \\\n      \\|    |/\n       | .  |           Thorny problems in Numerical Relativity and Beyond\n       |    |-\n       \\    /     Credits: Tom Goodale, Joan Masso, Paul Walker and Gabrielle Allen\n      --------        Including contributions from many worldwide collaborators\n      \\______/\n                ";

  CCTK_RegisterBanner(string);

}
