
int Cactus_Startup(void)
{
  
  const char *string;

  string = "
        \\ |/                                              
        /--\\                                              
      \\|   .|-                    \\  |  |  |  /  
       | .  |                                             
       | .  |-                   -  Cactus 4.0  -  
      -|    |                                             
       |  / |                     /  |  |  |  \\ 
      \\|    |/                                               
       | .  |           Thorny problems in Numerical Relativity and Beyond
       |    |-                                            
       \\    /     Credits: Tom Goodale, Joan Masso, Paul Walker and Gabrielle Allen 
      --------        Including contributions from many worldwide collaborators 
      \\______/  
                ";

  CCTK_RegisterBanner(string);

}
