#!/usr/bin/perl


use strict;
use warnings;
use Data::Dumper;
use IO::File;

my $FSSEP=",,,";
#0       1        2            3            4             5             6            7
#defline sequence [refseq 0mm] [refseq 1mm] [unigene 0mm] [unigene 1mm] [genome 0mm] [genome 1mm]

my $maxRecordsPerCategory=10;

sub addToArray{
  my ($array,$string) = @_;
  print Dumper($array);
  print Dumper($string);

  die;
  my @arrayt=split(",,,",$string);
  if($#arrayt > 0){
    push(@{$array},@arrayt);
  }else{
    push(@{$array},$string);
  }
}

sub pickSubArray{
  my (@array) = @_;

  my @arrayToReturn;
  my %randomidx;
  while( (scalar keys %randomidx) < $maxRecordsPerCategory){
    my $x=int(rand($#array+1));
    if(!(exists $randomidx{$x})){
      $randomidx{$x}=0;
      push(@arrayToReturn,$array[$x]);
    }
  }

  return @arrayToReturn;
}

my $numberOfFiles=0;
my @arrayOfFH;

#print $numberOfFiles;
foreach my $file (@ARGV){
  warn "Opening ". $file."\n";
  my $fh = new IO::File;
  $fh->open($file) or die "Cannot open file ".$file."\n";
  #open(FILEIN,$file) or die "Cannot open file ".$file."\n";
  push(@arrayOfFH,$fh);
  $numberOfFiles++;
}

my $line;
my @tempArray;
my $defline;
my $seq;
my $fh=$arrayOfFH[0];
while($line=<$fh>){
  #chomp($line);
  @tempArray = split("\t",$line);
  chomp($tempArray[7]);
  #print  "DUMP ".Dumper(@tempArray);
  $defline    = $tempArray[0];
  $seq        = $tempArray[1];

  my @refseq0mm  = split(",,,",$tempArray[2]);
  my @refseq1mm  = split(",,,",$tempArray[3]);
  my @unigene0mm = split(",,,",$tempArray[4]);
  my @unigene1mm = split(",,,",$tempArray[5]);
  my @genome0mm  = split(",,,",$tempArray[6]);
  my @genome1mm  = split(",,,",$tempArray[7]);

  for(my $i=1;$i<$numberOfFiles;$i++){
    $fh=$arrayOfFH[$i];
    $line = <$fh>;

    @tempArray = split("\t",$line);
    chomp($tempArray[7]);

    if($defline ne $tempArray[0]){
      die "Defline $defline not found\n";
    }
    if($seq     ne $tempArray[1]){
      die "Sequence $seq does not match\n";
    }

    push(@refseq0mm, split(",,,",$tempArray[2]));
    push(@refseq1mm, split(",,,",$tempArray[3]));
    push(@unigene0mm,split(",,,",$tempArray[4]));
    push(@unigene1mm,split(",,,",$tempArray[5]));
    push(@genome0mm, split(",,,",$tempArray[6]));
    push(@genome1mm, split(",,,",$tempArray[7]));
    #print "0mm ".Dumper(@genome0mm);
    #print "1mm ".Dumper(@genome1mm);
    #addToArray(\@refseq0mm, $tempArray[2]);
    #addToArray(\@refseq1mm, $tempArray[3]);
    #addToArray(\@unigene0mm,$tempArray[4]);
    #addToArray(\@unigene1mm,$tempArray[5]);
    #addToArray(\@genome0mm, $tempArray[6]);
    #addToArray(\@genome1mm, $tempArray[7]);

  }

  my $initialNumOfrefseq0mm  = ($#refseq0mm+1);
  my $initialNumOfrefseq1mm  = ($#refseq1mm+1);
  my $initialNumOfunigene0mm = ($#unigene0mm+1);
  my $initialNumOfunigene1mm = ($#unigene1mm+1);
  my $initialNumOfgenome0mm  = ($#genome0mm+1);
  my $initialNumOfgenome1mm  = ($#genome1mm+1);


  if($#refseq0mm >=  $maxRecordsPerCategory ){
    @refseq0mm=pickSubArray(@refseq0mm);
  }
  if($#refseq1mm >=  $maxRecordsPerCategory ){
    @refseq1mm=pickSubArray(@refseq1mm);
  }

  if($#unigene0mm >= $maxRecordsPerCategory ){
    @unigene0mm=pickSubArray(@unigene0mm);
  }
  if($#unigene1mm >= $maxRecordsPerCategory ){
    @unigene1mm=pickSubArray(@unigene1mm);
  }

  if($#genome0mm >=  $maxRecordsPerCategory ){
    @genome0mm=pickSubArray(@genome0mm);
  }
  if($#genome1mm >=  $maxRecordsPerCategory ){
    @genome1mm=pickSubArray(@genome1mm);
  }

  #print Dumper(@genome1mm);
  #print "###".join($FSSEP, @genome1mm)."####\n";
  print $defline."\t".$seq."\t".

    join($FSSEP, @refseq0mm)."\t".
      "(".$initialNumOfrefseq0mm.")\t".

      join($FSSEP, @refseq1mm)."\t".
	"(".$initialNumOfrefseq1mm.")\t".

	join($FSSEP, @unigene0mm)."\t".
	  "(".$initialNumOfunigene0mm.")\t".

	  join($FSSEP, @unigene1mm)."\t".
	    "(".$initialNumOfunigene1mm.")\t".

	    join($FSSEP, @genome0mm)."\t".
	      "(".$initialNumOfgenome0mm.")\t".

		join($FSSEP, @genome1mm)."\t".
		  "(".$initialNumOfgenome1mm.")\n";
  #print "######\n";

  $fh=$arrayOfFH[0];
}

foreach my $fh (@arrayOfFH){
  $fh->close();
}





