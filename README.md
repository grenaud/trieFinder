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
    tagID	tagseq	0mmRefseq	(#0mmRefseq)	>0mmRefseq	(#>0mmRefseq)	0mmUnigene	(#0mmUnigene)	>0mmUnigene	(#>0mmUnigene)	0mmGenome	(#0mmGenome)
>0mmGenome	(#>0mmGenome)
Output:
    @seq1	GATCGTCTGGAGCCTCCTCAAATGACACAC		(0)		(0)	>gnl|UG|Dr#S12243245:+,,,>gnl|UG|Dr#S36304394:+,,,>gnl|UG|Dr#S36299574:+,,,>gnl|UG|Dr#S12193130:+,,,>gnl|UG|Dr#S36316063:+,,,>gnl|UG|Dr#S12121054:+,,,>gnl|UG|Dr#S34224301:+,,,>gnl|UG|Dr#S12124397:+,,,>gnl|UG|Dr#S36315971:-,,,>gnl|UG|Dr#S21009866:-,,,>gnl|UG|Dr#S18222838:-,,,>gnl|UG|Dr#S12125085:-,,,>gnl|UG|Dr#S18221142:-	(26)		(0)		(0)
    @seq2	GATCATGCCGCCGCAGCCATCGCCAAAACC		(0)		(0)	>gnl|UG|Dr#S36280454:+,,,>gnl|UG|Dr#S46923620:+,,,>gnl|UG|Dr#S24522822:+,,,>gnl|UG|Dr#S34306077:+,,,>gnl|UG|Dr#S24523153:+,,,>gnl|UG|Dr#S19528905:+,,,>gnl|UG|Dr#S52625016:+,,,>gnl|UG|Dr#S26412454:+,,,>gnl|UG|Dr#S12281713:+,,,>gnl|UG|Dr#S19069881:+,,,>gnl|UG|Dr#S18655929:+,,,>gnl|UG|Dr#S21532139:+,,,>gnl|UG|Dr#S16007879:+,,,>gnl|UG|Dr#S26411042:+,,,>gnl|UG|Dr#S15988909:+	(6)		(0)		(0)
    @seq3	GATCAGACCAGCAACAAACTACAGCATTCA	>gi|47087442|ref|NM_213455.1|:+,,,>gnl|UG|Dr#S32062939:+,,,>gnl|UG|Dr#S16155405:+,,,>gnl|UG|Dr#S31888623:+,,,>gnl|UG|Dr#S36329495:+,,,>gnl|UG|Dr#S12247787:+,,,>gnl|UG|Dr#S31884660:+,,,>gnl|UG|Dr#S26113583:+	(10)		(0)		(0)		(0)
    @seq4	GATCCAGAAGAGCGGGACAAAATAAATGCC	>gi|326668314|ref|XM_692109.5|:+	(1)		(0)		(0)	(0)		(0)		(0)
    @seq5	GATCTGGCAGTATGGCAAACCAAACATCTA	>gi|326669953|ref|XM_001341168.3|:+	(1)		(0)	>gnl|UG|Dr#S12167163:+	(1)		(0)		(0)		(0)
    @seq6	GATCGAGCAATCACTGGTCACTCACGCAGC		(0)		(0)	>gnl|UG|Dr#S12184648:+,,,>gnl|UG|Dr#S18653545:+,,,>gnl|UG|Dr#S36222728:+,,,>gnl|UG|Dr#S36233154:+,,,>gnl|UG|Dr#S12283737:+,,,>gnl|UG|Dr#S27069424:+,,,>gnl|UG|Dr#S14274622:+,,,>gnl|UG|Dr#S12226371:+,,,>gnl|UG|Dr#S27073093:+,,,>gnl|UG|Dr#S27059264:+,,,>gnl|UG|Dr#S12181595:+,,,>gnl|UG|Dr#S12110458:+	(3)		(0)		(0)
    @seq7	GATCTCACTCCTCTCGCCACTGCGTACGCC	>gi|195539483|ref|NM_001082796.2|:+,,,>gnl|UG|Dr#S14267648:+,,,>gnl|UG|Dr#S12210562:+,,,>gnl|UG|Dr#S24604573:+,,,>gnl|UG|Dr#S12369849:+,,,>gnl|UG|Dr#S37452744:+,,,>gnl|UG|Dr#S12256675:+,,,>gnl|UG|Dr#S31851124:+,,,>gnl|UG|Dr#S17837918:+,,,>gnl|UG|Dr#S31869489:+,,,>gnl|UG|Dr#S35356608:+,,,>gnl|UG|Dr#S12100358:+	(3)		(0)		(0)
    @seq8	GATCTTCTCAGAACTCCAGTATTTGATCAA	>gi|225007545|ref|NM_001083546.2|:+,,,>gi|326672464|ref|XM_002663974.2|:+	(2)		(0)		(0)		(0)		(0)		(0)
    @seq9	GATCTCCAACAGTTCTTTAGGAGTAGAGCA	>gi|326669327|ref|XM_678994.5|:+	(1)		(0)		(0)	(0)		(0)		(0)
    @seq10	GATCAGTGGAGTGTCTGGTGATAAAGCTGC		(0)		(0)	>gnl|UG|Dr#S12078550:+,,,>gnl|UG|Dr#S19861994:+,,,>gnl|UG|Dr#S32027677:+,,,>gnl|UG|Dr#S34830601:+,,,>gnl|UG|Dr#S41574179:+,,,>gnl|UG|Dr#S46922454:+,,,>gnl|UG|Dr#S12113846:-	(10)		(0)	(0)


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
