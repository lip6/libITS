#!/usr/bin/perl

my %groups;

# read on the standard input
while (my $line=<STDIN>)
{
	chomp $line;
	my @model = split (/\./,$line);
	$groups{@model[0]}->{@model[1]} = 1;
}

# for each type of model
foreach $key (sort keys %groups)
{
	# creates a graph for the current type of model
	print "    <graph title=\"Time for $key\" defaultFilters=\"\" hideFilters=\"\">\n";
	# for each instance of the model
	foreach $instance (sort keys %{ $groups{$key} })
	{
		# add a value to be printed on the graph
		print "      <valueType key=\"testDuration_$key.$instance.dve\" title=\"$key.$instance time (s)\" buildTypeId=\"bt141\" />\n";
	}
	# close
	print "    </graph>\n";
	# idem for the memory stats
	print "    <graph title=\"Memory for $key\" defaultFilters=\"\" hideFilters=\"\">\n";
	foreach $instance (sort keys %{$groups{$key}})
	{
		print "      <valueType key=\"testMemory_$key.$instance.dve\" title=\"$key.$instance memory (kb)\" buildTypeId=\"bt141\" />\n";
	}
	print "    </graph>\n";
}
