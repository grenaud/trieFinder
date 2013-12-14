trieFinder
==========


trieFinder is a software suite for the automatic alignment of tags stemming from Massively Parallel Signature Sequencing (MPSS) to preannotated databases of putative sequence tags from both the genome and transcriptome ( Refseq and Unigene ).


See the readme.pdf for installation, running and miscellaneous documentation.


Example of usage
==========

Given short MPSS tags that were sequenced :

    @seq1
    GATCGTCTGGAGCCTCCTCAAATGACACAC
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq2
    GATCATGCCGCCGCAGCCATCGCCAAAACC
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq3
    GATCAGACCAGCAACAAACTACAGCATTCA
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq4
    GATCCAGAAGAGCGGGACAAAATAAATGCC
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq5
    GATCTGGCAGTATGGCAAACCAAACATCTA
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq6
    GATCGAGCAATCACTGGTCACTCACGCAGC
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq7
    GATCTCACTCCTCTCGCCACTGCGTACGCC
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq8
    GATCTTCTCAGAACTCCAGTATTTGATCAA
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq9
    GATCTCCAACAGTTCTTTAGGAGTAGAGCA
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    @seq10
    GATCAGTGGAGTGTCTGGTGATAAAGCTGC
    +
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

trieFinder will report the hits across 3 different databases for those tags, one line per tag, where the hits are prioritized according to the database, where a certain number of hits are reported along with the strand and the total number of hits for this database  :

Columns:

    tagID	tagseq	0_mismatch_Refseq	(#0_mismatch_Refseq)	>0_mismatch_Refseq	(#>0_mismatch_Refseq)	0_mismatch_Unigene	(#0_mismatch_Unigene)	>0_mismatch_Unigene	(#>0_mismatch_Unigene)	0_mismatch_Genome	(#0_mismatch_Genome)	>0_mismatch_Genome	(#>0_mismatch_Genome)

Column meaning:


    -------------------------------------------------------------------------------------------
    | Column           | Meaning                                                              |
    -------------------------------------------------------------------------------------------
    | tagID            |  tag identifier                                                      | 
    | tagseq           |  tag sequence                                                        | 
    | 0_mm_Refseq      |  Identifier of the RefSeq(s) hit with 0 mismatches                   | 
    | (#0_mm_Refseq)   |  Number of RefSeq(s) hit with 0 mismatches                           | 
    | >0_mm_Refseq     |  Identifier of the RefSeq(s) hit with 1 or more mismatches           | 
    | (#>0_mm_Refseq)  |  Number of RefSeq(s) hit with 1 or more mismatches                   | 
    | 0_mm_Unigene     |  Identifier of the UniGene sequence(s) hit with 0 mismatches         | 
    | (#0_mm_Unigene)  |  Number of UniGene sequence(s) hit with 0 mismatches                 | 
    | >0_mm_Unigene    |  Identifier of the UniGene sequence(s) hit with 1 or more mismatches | 
    | (#>0_mm_Unigene) |  Number of UniGene sequence(s) hit with 1 or more mismatches         | 
    | 0_mm_Genome      |  Identifier of the Genome sequence(s) hit with 0 mismatches          | 
    | (#0_mm_Genome)   |  Number of  Genome sequence(s) hit with 0 mismatches                 | 
    | >0_mm_Genome     |  Identifier of the  Genome sequence(s) hit with 1 or more mismatches | 
    | (#>0_mm_Genome)  |  Number of  Genome sequence(s) hit with 1 or more mismatches         |   
    -------------------------------------------------------------------------------------------

Output:


    @seq1	GATCGTCTGGAGCCTCCTCAAATGACACAC		(0)		(0)	>gnl|UG|Dr#S12243245:432:-,,,>gnl|UG|Dr#S12151044:253:+,,,>gnl|UG|Dr#S36304394:658:-,,,>gnl|UG|Dr#S12244676:107:+,,,>gnl|UG|Dr#S36299574:226:+,,,>gnl|UG|Dr#S12193130:616:-,,,>gnl|UG|Dr#S34232649:231:+,,,>gnl|UG|Dr#S36316063:668:-,,,>gnl|UG|Dr#S36280735:596:-,,,>gnl|UG|Dr#S18343704:651:-	(10)	>gnl|UG|Dr#S12124423:111:+,,,>gnl|UG|Dr#S12121054:618:-,,,>gnl|UG|Dr#S34224583:229:+,,,>gnl|UG|Dr#S34224301:234:+,,,>gnl|UG|Dr#S12124397:24:+,,,>gnl|UG|Dr#S36315971:655:-,,,>gnl|UG|Dr#S21009866:626:-,,,>gnl|UG|Dr#S18222838:626:-,,,>gnl|UG|Dr#S12125085:620:-,,,>gnl|UG|Dr#S18221142:649:-	(26)		(0)		(0)
    @seq2	GATCATGCCGCCGCAGCCATCGCCAAAACC		(0)		(0)	>gnl|UG|Dr#S36280454:411:+,,,>gnl|UG|Dr#S46923620:289:+,,,>gnl|UG|Dr#S24522822:344:+,,,>gnl|UG|Dr#S34306077:328:+,,,>gnl|UG|Dr#S24523153:348:+,,,>gnl|UG|Dr#S19528905:350:+,,,>gnl|UG|Dr#S52625016:213:+,,,>gnl|UG|Dr#S26412454:367:+,,,>gnl|UG|Dr#S12281713:329:+,,,>gnl|UG|Dr#S19069881:355:+	(10)	>gnl|UG|Dr#S31842702:219:+,,,>gnl|UG|Dr#S18655929:336:+,,,>gnl|UG|Dr#S21532139:379:+,,,>gnl|UG|Dr#S16007879:388:+,,,>gnl|UG|Dr#S26411042:285:+,,,>gnl|UG|Dr#S15988909:325:+	(6)		(0)		(0)
    @seq3	GATCAGACCAGCAACAAACTACAGCATTCA	>gi|47087442|ref|NM_213455.1|:1755:+	(1)		(0)	>gnl|UG|Dr#S26113934:491:+,,,>gnl|UG|Dr#S32062939:592:+,,,>gnl|UG|Dr#S16155405:386:+,,,>gnl|UG|Dr#S31888623:819:+,,,>gnl|UG|Dr#S36329495:711:-,,,>gnl|UG|Dr#S32029339:731:-,,,>gnl|UG|Dr#S31881829:118:+,,,>gnl|UG|Dr#S12247787:64:+,,,>gnl|UG|Dr#S31884660:575:+,,,>gnl|UG|Dr#S26113583:550:+	(10)		(0)	(0)		(0)
    @seq4	GATCCAGAAGAGCGGGACAAAATAAATGCC	>gi|326668314|ref|XM_692109.5|:103:+	(1)		(0)		(0)		(0)		(0)		(0)
    @seq5	GATCTGGCAGTATGGCAAACCAAACATCTA	>gi|326669953|ref|XM_001341168.3|:833:+	(1)	(0)	>gnl|UG|Dr#S12167163:184:+	(1)		(0)		(0)		(0)
    @seq6	GATCGAGCAATCACTGGTCACTCACGCAGC		(0)		(0)	>gnl|UG|Dr#S12184648:64:+,,,>gnl|UG|Dr#S18653545:44:+,,,>gnl|UG|Dr#S36222728:143:+,,,>gnl|UG|Dr#S36233154:90:+,,,>gnl|UG|Dr#S12283737:131:+,,,>gnl|UG|Dr#S27069424:105:+,,,>gnl|UG|Dr#S14274622:60:+,,,>gnl|UG|Dr#S12226371:132:+,,,>gnl|UG|Dr#S27073093:84:+,,,>gnl|UG|Dr#S27059264:66:+	(10)	>gnl|UG|Dr#S12120057:53:+,,,>gnl|UG|Dr#S12181595:60:+,,,>gnl|UG|Dr#S12110458:53:+	(3)		(0)		(0)
    @seq7	GATCTCACTCCTCTCGCCACTGCGTACGCC	>gi|195539483|ref|NM_001082796.2|:923:+	(1)		(0)	>gnl|UG|Dr#S34289349:197:+,,,>gnl|UG|Dr#S14267648:971:+,,,>gnl|UG|Dr#S12210562:362:+,,,>gnl|UG|Dr#S24604573:34:+,,,>gnl|UG|Dr#S12369849:157:+,,,>gnl|UG|Dr#S37452744:923:+,,,>gnl|UG|Dr#S12256675:220:+,,,>gnl|UG|Dr#S31851124:410:+,,,>gnl|UG|Dr#S17837918:946:+,,,>gnl|UG|Dr#S31869489:75:+	(10)	>gnl|UG|Dr#S16157603:731:+,,,>gnl|UG|Dr#S35356608:763:+,,,>gnl|UG|Dr#S12100358:625:+	(3)		(0)		(0)
    @seq8	GATCTTCTCAGAACTCCAGTATTTGATCAA	>gi|225007545|ref|NM_001083546.2|:1942:+,,,>gi|326672464|ref|XM_002663974.2|:1942:+	(2)		(0)		(0)		(0)		(0)		(0)
    @seq9	GATCTCCAACAGTTCTTTAGGAGTAGAGCA	>gi|326669327|ref|XM_678994.5|:2338:+	(1)		(0)		(0)		(0)		(0)		(0)
    @seq10	GATCAGTGGAGTGTCTGGTGATAAAGCTGC		(0)		(0)	>gnl|UG|Dr#S12078550:307:-,,,>gnl|UG|Dr#S16546511:1672:+,,,>gnl|UG|Dr#S19861994:1672:+	(3)	>gnl|UG|Dr#S12098337:317:-,,,>gnl|UG|Dr#S12166308:321:-,,,>gnl|UG|Dr#S15997299:323:-,,,>gnl|UG|Dr#S20967007:1675:+,,,>gnl|UG|Dr#S32027677:337:-,,,>gnl|UG|Dr#S32054770:274:+,,,>gnl|UG|Dr#S34830601:295:+,,,>gnl|UG|Dr#S41574179:1684:+,,,>gnl|UG|Dr#S46922454:1659:+,,,>gnl|UG|Dr#S12113846:285:-	(10)		(0)		(0)




License
==========

Copyright (C) 2013 Gabriel Renaud

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
