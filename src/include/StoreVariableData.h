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

int CCTK_GroupNumber(const char *implementation,
			const char *name);


int CCTK_VarNumber(const char *implementation,
			   const char *group_name,
			   const char *variable_name);

int CCTK_MaxDim(void);

int CCTK_NumVars(void);

int CCTK_NumGroups(void);


