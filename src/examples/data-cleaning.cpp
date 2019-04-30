#ifndef DATACLEANING_CPP
#define DATACLEANING_CPP

#include <cstdio>
#include <string>
#include <iostream>
#include <list>

/*!
 * \brief Point filter class
 */
class PointFilter{
   public:
       
       /**Create a point filter*/
	PointFilter(){

	}

        /**Destroy the point filter*/
	~PointFilter(){

	}
        
        /**
         * return true if we remove this point
         * 
         * @param microEpoch timestamp of the point
         * @param x x position of the point
         * @param y y position of the point
         * @param z z position of the point
         * @param quality quality of the point
         * @param intensity intensity of the point
         */
	virtual bool filterPoint(uint64_t microEpoch,double x,double y,double z, uint32_t quality,uint32_t intensity) = 0;
};

/*!
 * \brief Quality filter class extend of the Point filter class
 */
class QualityFilter : public PointFilter{
   public:

       /**
        * Create a quality filter
        * 
        * @param minimumQuality the minimal quality accepted
        */
	QualityFilter(int minimumQuality) : minimumQuality(minimumQuality){

	}

        /**Destroy the quality filter*/
	~QualityFilter(){

	}

        /**
         * return true if the quality receive is low then the minimum accepted
         * 
         * @param microEpoch timestamp of the point
         * @param x x position of the point
         * @param y y position of the point
         * @param z z position of the point
         * @param quality quality of the point
         * @param intensity intensity of the point
         */
	bool filterPoint(uint64_t microEpoch,double x,double y,double z, uint32_t quality,uint32_t intensity){
		return quality < minimumQuality;
	}

  private:
        
      /**minimal quality accepted*/
	unsigned int minimumQuality;

};

/**
 * Filter all point receive in the terminal
 * 
 * @param argc number of parameter
 * @param argv value of the parameters
 */
int main(int argc,char** argv){

	std::string line;

	//Filter chain
	std::list<PointFilter *> filters;

	//TODO: load desired filters and parameters from command line
        if (argc < 2)
        {
            filters.push_back(new QualityFilter(0));
        }
        else
        {
            int i = 1;
            int quality;
            while(i < argc)
            {
                
                if (std::sscanf(argv[i],"%d",&quality)==1)
                {
                    filters.push_back(new QualityFilter(quality));
                }
                else
                {
                    std::cerr << "Error: parameter " << argv[i] << " invalid" << std::endl;
                }
                i = i+1;
            }
        }
        unsigned int lineCount = 1;
        while((std::getline(std::cin,line))&&(line!="0")){
            uint64_t microEpoch;
            double x,y,z;
            uint32_t quality;
            uint32_t intensity;

            if(sscanf(line.c_str(),"%lu %lf %lf %lf %d %d",&microEpoch,&x,&y,&z,&quality,&intensity)==6){
		bool doFilter = false;

			//Apply filter chain
		for(auto i = filters.begin();i!= filters.end();i++){
                    if( (*i)->filterPoint(microEpoch,x,y,z,quality,intensity) ){
			doFilter = true;
                        break;
                    }
		}

		if(!doFilter){
                    printf("%lu %.6lf %.6lf %.6lf %d %d\r\n",microEpoch,x,y,z,quality,intensity);
		}
            }
            else{
		std::cerr << "Error at line " << lineCount << std::endl;
            }
                
            lineCount++;
        }
    }
#endif
