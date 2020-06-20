
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

int main( void )
{
   if( _mkdir( "C:/testtmp/xd" ) == 0 )
   {
      printf( "Directory 'C:/testtmp/xd' was successfully created\n" );
      system( "dir C:/testtmp/xd" );
      if( _rmdir( "C:/testtmp/xd" ) == 0 )
        printf( "Directory 'C:/testtmp/xd' was successfully removed\n"  );
      else
         printf( "Problem removing directory 'C:/testtmp/xd'\n" );
   }
   else
      printf( "Problem creating directory 'C:/testtmp/xd'\n" );
}