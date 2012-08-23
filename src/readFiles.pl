#!/usr/bin/perl


use strict;
use warnings;
use Data::Dumper;

my $maxRecordsPerCategory=10;

sub minString{
  my (@array) = @_;
  @array=  sort(@array);
  return $array[0];
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

if($#ARGV == -1){
  die "Usage ./readFile.pl [restenz] [nuc] [mm?0:1] [prefixRF] [prefixUG] [prefixWG] [tempdir]\n";
}

my $restenz =$ARGV[0]; #e.g GATC
my $nuc     =$ARGV[1]; #A,C,G,T
my $onemm   =$ARGV[2]; # 0 no mm, 1 = with mm
my $prefixRF=$ARGV[3];
my $prefixUG=$ARGV[4];
my $prefixWG=$ARGV[5];
my $tempdir =$ARGV[6];
my $lengthS =$ARGV[7];

my $fileRF0;
my $fileUG0;
my $fileWG0;

if($onemm != 0 && $onemm != 1){
  die "One mismatch has to be 0 or 1\n";
}

if($onemm == 1 ){
  $fileRF0=$tempdir.$prefixRF.".".$restenz.".l".$lengthS.".mm1.out".$nuc.".gz";
  $fileUG0=$tempdir.$prefixUG.".".$restenz.".l".$lengthS.".mm1.out".$nuc.".gz";
  $fileWG0=$tempdir.$prefixWG.".".$restenz.".l".$lengthS.".mm1.out".$nuc.".gz";
}else{

  $fileRF0=$tempdir.$prefixRF.".".$restenz.".l".$lengthS.".mm0.out.gz";
  $fileUG0=$tempdir.$prefixUG.".".$restenz.".l".$lengthS.".mm0.out.gz";
  $fileWG0=$tempdir.$prefixWG.".".$restenz.".l".$lengthS.".mm0.out.gz";
}



open(RF, "gunzip -c $fileRF0 |") || die "can't open pipe to $fileRF0";
open(UG, "gunzip -c $fileUG0 |") || die "can't open pipe to $fileUG0";
open(WG, "gunzip -c $fileWG0 |") || die "can't open pipe to $fileWG0";
my $tagRF;
my $tagUG;
my $tagWG;
my $annoRF;
my $annoUG;
my $annoWG;

my $lineRF = <RF>;
my $lineUG = <UG>;
my $lineWG = <WG>;

my $endRF = 0;
my $endUG = 0;
my $endWG = 0;


#hopefully we have at least one record for each file
if ($lineRF =~ /^(.*)\t(\d+)\t(\w+)\t([+-])$/) {
  $tagRF = $3;
  $annoRF= $1."#".$2."#".$4;
} else {
  die "RF line = ".$lineRF." did not parse\n";
}

if ($lineUG =~ /^(.*)\t(\d+)\t(\w+)\t([+-])$/) {
  $tagUG = $3;
  $annoUG= $1."#".$2."#".$4;
} else {
  die "UG line = ".$lineUG." did not parse\n";
}


if ($lineWG =~ /^(.*)\t(\d+)\t(\w+)\t([+-])$/) {
  $tagWG = $3;
  $annoWG= $1."#".$2."#".$4;
} else {
  die "WG line = ".$lineWG." did not parse\n";
}


while (1) {
  if ($endRF &&
      $endUG &&
      $endWG) {
    last;
  }
  my @arrayOfTags;
  my @toprintRF;
  my @toprintUG;
  my @toprintWG;

  if (!$endRF) {
    push(@arrayOfTags,$tagRF);
  }

  if (!$endUG) {
    push(@arrayOfTags,$tagUG);
  }

  if (!$endWG) {
    push(@arrayOfTags,$tagWG);
  }

  my $minS=minString(@arrayOfTags);

  if (!$endRF &&
      $minS eq $tagRF) {

    while ($minS eq $tagRF) {
      push(@toprintRF,$annoRF);
      if ( !( $lineRF = <RF>) ) {
	$endRF=1;
	last;
      }

      if ($lineRF =~ /^(.*)\t(\d+)\t(\w+)\t([+-])$/) {
	$tagRF = $3;
	$annoRF= $1."#".$2."#".$4;
      } else {
	die "RF line = ".$lineRF." did not parse\n";
      }
    }
  }

  if (!$endUG &&
      $minS eq $tagUG) {

    while ($minS eq $tagUG) {
      push(@toprintUG,$annoUG);
      if ( !( $lineUG = <UG>) ) {
	$endUG=1;
	last;
      }

      if ($lineUG =~ /^(.*)\t(\d+)\t(\w+)\t([+-])$/) {
	$tagUG = $3;
	$annoUG= $1."#".$2."#".$4;
      } else {
	die "UG line = ".$lineUG." did not parse\n";
      }
    }
  }

  if (!$endWG &&
      $minS eq $tagWG) {

    while ($minS eq $tagWG) {
      push(@toprintWG,$annoWG);
      if ( !( $lineWG = <WG>) ) {
	$endWG=1;
	last;
      }

      if ($lineWG =~ /^(.*)\t(\d+)\t(\w+)\t([+-])$/) {
	$tagWG = $3;
	$annoWG= $1."#".$2."#".$4;
      } else {
	die "WG line = ".$lineWG." did not parse\n";
      }
    }
  }

  #my @toprintRF2;
  #my @toprintUG2;
  #my @toprintWG2;
  if(0){
    if($#toprintRF >= 10 ){
      @toprintRF=pickSubArray(@toprintRF);
    }#else{
    # @toprintRF2=@toprintRF;
    #}

    if($#toprintUG >= 10 ){
      @toprintUG=pickSubArray(@toprintUG);
    }#else{
    # @toprintUG2=@toprintUG;
    #}

    if($#toprintWG >= 10 ){
      @toprintWG=pickSubArray(@toprintWG);
    }#else{
    # @toprintWG2=@toprintWG;
    #}
  }
  print
    $minS
      ."\t".
	join("###",@toprintRF)
	  ."\t".
	    join("###",@toprintUG)
	      ."\t".
		join("###",@toprintWG)
		  ."\n";

}				#end while

close(RF);
close(UG);
close(WG);

