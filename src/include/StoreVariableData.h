 /*@@
   @header    StoreVariableData.h
   @date      Fri Jan 15 13:55:26 1999
   @author    Tom Goodale
   @desc 
   
   @enddesc
   @version $Id$
 @@*/

cGroupDefinition *CCTK_SetupGroup(const char *implementation, 
				  const char *name, 
				  int n_variables);

int CCTK_GetGroupNumber(const char *implementation,
			const char *name);


int CCTK_GetVariableNumber(const char *implementation,
			   const char *group_name,
			   const char *variable_name);

int CCTK_GetMaxDim(void);

int CCTK_GetNVariables(void);

int CCTK_GetNGroups(void);


