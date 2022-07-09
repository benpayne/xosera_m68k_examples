#!/usr/bin/perl

@include_paths = split( " ", $ENV{ "HPATH" } );

foreach ( @include_paths )
{
   s/\/([a-z])\//$1:\//;
}

#print STDERR ($ENV{ "HPATH" } . " \n" );

%filehash;

my $included;

foreach $file ( @ARGV )
{
    $filehash{ $file } = {};

	#print STDERR ( "process $file\n" );
    process_file( $file, $file, [] );
}

foreach $file ( sort( keys( %filehash ) ) )
{
    my $file_hash, $path_fix;
	#print STDERR ( "generate $file\n" );
    if ( $file =~ /^(..\/)*(.*)\.(c(|pp|xx)|idl|asm|S)$/ )
    {
		$file_hash = $filehash{ $file };
		$path_fix = $2;
		if ( $3 ne "idl" ) { 
			print "\$(OBJ_DIR)/$2\$(OBJECT_EXT): $file";
		} else {
			print "\$(IDLDIR)/$2.h: $file";
		}

		print "\\\n\t\$(MAKEFILE_DEPS)";

		foreach $include ( keys( %$file_hash ) )
		{
			$include =~ s/([a-zA-Z]):(\/|\\)/\/\/$1\//;
	
			if ( $2 ne "idl" and $include =~ /^((.*)\/)?([^\.\/]*)\.idl$/ ){
				print " \\\n\t$2/\$(IDLDIR)/$3.h";
			}else{
				print " \\\n\t$include";
			}
		}
	
		if ( $file =~ /([^\.]*)\.(c(pp|xx))$/ ) {
			print "\n\t\$(PRE_BUILD_RULE)\n\t\$(CXX_BUILD_RULE)\n\n";
		} elsif ( $file =~ /([^\.]*)\.([cS])$/ ) {
			print "\n\t\$(PRE_BUILD_RULE)\n\t\$(C_BUILD_RULE)\n\n";
		} elsif ( $file =~ /([^\.]*)\.(asm)$/ ) {
			print "\n\t\$(PRE_BUILD_RULE)\n\t\$(NASM_BUILD_RULE)\n\n";
		} else {
			print "\n\n";
		}
	}
}

#print STDERR ( "done\n" );
#exit( 0 );

sub process_file
{
    my $master_file = $_[ 0 ];
    my $file = $_[ 1 ];
    my $included = $_[ 2 ];
    my @files_found;
    my $include, $path;

    if ( defined search_array( $included, $file ) )
    {
#		print STDERR ( "$master_file:$file already included\n" ); 
		return;
    }
    if ( defined $includehash{ $file } )
    { 
#		print STDERR ( "$master_file:$file parsed, including includes\n" ); 
		push( @$included, $file );
		foreach $path ( @{$includehash{ $file }} )
		{
			add_include_to_file( $master_file, $path );
		}
		foreach $path ( @{$includehash{ $file }} )
		{
			process_file( $master_file, $path, $included );
		}
		return;
    }

#    print STDERR ( "$master_file:$file parsing file\n" ); 
    $includehash{ $file } = [];
    push( @$included, $file );
    
    open( FILE, $file ) or die( "failed to open file $file\n" );
#    print "$file\n";
    while( <FILE> )
    {
		if ( /^\#[ \t]*include[ \t]*\"([^\"]*)\"/ )
		{
			$path = find_file( $1 );
			if ( not defined $path )
			{
				print STDERR ( "WARNING: $1 could not be found\n\tfrom $file\n\tin $master_file\n" );
			}
			else
			{
				add_include_to_file( $master_file, $path );
				push( @{$includehash{ $file }}, $path );
			}
		}
    }
    foreach $include ( @{$includehash{ $file }} )
    {
		process_file( $master_file, $include, $included );
    }
}

sub add_include_to_file()
{
    my $master_file = $_[ 0 ]; 
    my $path  = $_[ 1 ];
    my $file_hash = $filehash{ $master_file };

    $file_hash->{ $path } = 1;
#    push( @{$filehash{ $master_file }}, $path );
}

sub search_array()
{
    $array = $_[ 0 ];
    $val = $_[ 1 ];

    foreach ( @$array ){
#	print STDERR ( "\t$_ == $val\n" );
	if ( $_ eq $val ){
	    return 1;
	}
    }
    return undef;
}

sub find_file()
{
    my $file = $_[ 0 ];

    # hack to make the cmuser file get in the deps if it hasn't been generated yet
#    if ( $file eq "cmuser.h" ){
#	return $file;
#    } 

    if ( -e $file ){
	return $file;
    } elsif ( $file =~ /^([^\.]*)\.h$/ ) {
	if ( -e $1 . ".idl" ){
	    return $1 . ".idl";
	}
    }   
    
    foreach $path ( @include_paths )
    {
	if ( -e $path . "/" . $file ){
	    return $path . "/" . $file;
	} elsif ( $file =~ /^([^\.]*)\.h$/ ) {
	    if ( -e $path . "/" . $1 . ".idl" ){
		return $path . "/" . $1 . ".idl";
	    }
	}   
    }

    return undef;
}
