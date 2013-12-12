#!/usr/bin/perl

my %groups;

# read on the standard input
while (my $line=<STDIN>)
{
	chomp $line;
	my @model = split (/\./,$line);
	$groups{@model[0]}->{join ('.', @model[1..$#model])} = 1;
}

foreach $bt  ("bt141","bt169") {
# for each type of model
foreach $key (sort keys %groups)
{
  if ($bt =~ /bt169/ ) {
    $arch = "x64";
  } else {
    $arch = "i32";
  }
	# creates a graph for the current type of model
	print "    <graph title=\"Time for $key ($arch)\" defaultFilters=\"showFailed\" hideFilters=\"\" >\n";
	# for each instance of the model
	foreach $instance (sort keys %{ $groups{$key} })
	{
		# add a value to be printed on the graph
		print "      <valueType key=\"testDuration_$key.$instance\" title=\"$key.$instance time (s)\" buildTypeId=\"$bt\" />\n";
	}
	# close
	print "    </graph>\n";
	# idem for the memory stats
	print "    <graph title=\"Memory for $key ($arch)\" defaultFilters=\"showFailed\" hideFilters=\"\">\n";
	foreach $instance (sort keys %{$groups{$key}})
	{
		print "      <valueType key=\"testMemory_$key.$instance\" title=\"$key.$instance memory (kb)\" buildTypeId=\"bt141\" />\n";
	}
	print "    </graph>\n";
}
}
