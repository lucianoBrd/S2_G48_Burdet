# TP2-B

## MISRA C Coding Standard

MISRA (la Motor Industry Software Reliability Association) fournit des directives pour le développement de systèmes électroniques liés à la sûreté et à la sécurité, de systèmes de contrôle embarqués, d'applications logicielles intensives et de logiciels autonomes.

MISRA est le fruit d'un effort de collaboration entre les constructeurs automobiles, les fournisseurs de composants et les bureaux d'études. Il est géré par un comité directeur qui comprend Ford Motor Company, Bentley Motors, Jaguar Land Rover, HORIBA MIRA, ZF TRW et l'Université de Leeds.

Bien que née dans l'industrie automobile, MISRA a été acceptée sur d'autres marchés tels que l'aérospatiale, le biomédical et la finance. Il est également accepté dans les systèmes de contrôle embarqués, IoT et industriels. Bien que la conformité MISRA ne garantisse pas que le logiciel sera exempt de tout problème de qualité ou de sécurité, elle produit un code plus robuste, plus facile à maintenir et plus portable.

* R 2.7 - There should be no unused parameters in functions -> Advisory
* R 3.1 - The character sequences /* and // shall not be used within a comment -> Required
* R 13.4 - The result of an assignment operator should not be used -> Advisory

J'ai enlevé les erreurs suivantes :
* R 14.4 - The controlling expression of an if statement and the controlling expression of an iteration-statement shall have essentially Boolean type -> Required
* R 12.3 - The comma operator should not be used -> Advisory
* R 13.4 - The result of an assignment operator should not be used -> Advisory